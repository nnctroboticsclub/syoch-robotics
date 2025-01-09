#include <stdint.h>
#include <robotics/binary/linked-allocator.hpp>

#include "./config.h"

namespace robotics::binary::linked_allocator {

/// @brief sizeof(size_t) バイト境界で切り上げ
static size_t RoundUpSize(size_t size) {
  return (size + sizeof(size_t) - 1) & ~(sizeof(size_t) - 1);
}

/// @brief 2 のべき乗かどうかを判定する
template <typename T>
static bool IsPowerOf2(T value) {
  return (value & (value - 1)) == 0;
}

/// @brief 確保済みのチャンク
struct AllocatedChunk {
  /// @brief 前方リンク
  AllocatedChunk* prev;
  /// @brief メタデータ
  uint32_t manager_data;

  /// @brief チャンクのサイズを取得する
  [[nodiscard]] auto Size() const { return manager_data & ~0x3; }

  /// @brief チャンクのサイズを設定する
  void Size(uint32_t size) { manager_data = RoundUpSize(size); }

  /// @brief チャンクが解放済みかどうかを取得する
  [[nodiscard]] auto IsFreed() const { return manager_data & 0x1; }

  /// @brief チャンクが使用中かどうかを取得する
  [[nodiscard]] auto IsUsed() const { return !IsFreed(); }

  /// @brief チャンクを解放済みとしてマークする
  void MarkFreed() { manager_data |= 0x1; }

  /// @brief チャンクを使用中としてマークする
  void MarkUsed() { manager_data &= ~0x1; }

  /// @brief 次のチャンクを取得する
  [[nodiscard]] auto Next() {
    auto* ptr = reinterpret_cast<uint8_t*>(this);  // in byte
    ptr += Size() + sizeof(AllocatedChunk);

    return reinterpret_cast<AllocatedChunk*>(ptr);
  }

  /// @brief 前のチャンクを取得する
  [[nodiscard]] auto& Prev() const { return *this->prev; }

  /// @brief 前のチャンクを設定する
  void Prev(AllocatedChunk* ptr) { prev = ptr; }

  /// @brief データへのポインタを取得する
  [[nodiscard]] auto Data() { return static_cast<void*>(this + 1); }
};

bool Arena::InHeap(const void* ptr) const {
  auto val = reinterpret_cast<uint32_t>(ptr);

  auto begin = reinterpret_cast<uint32_t>(heap_start);
  auto end = begin + heap_size;

  auto ret = (begin <= val) && (val < end);

  if (!ret) {
    SR_LA_A_LOG("la/a: %xh~%xh o%xh(%d&%d=%d)\n", begin, end, val, begin <= val,
                val < end, ret);
  }

  return ret;
}

[[nodiscard]] static auto CalculateAlignScoreScore(const AllocatedChunk* chunk,
                                                   uint32_t needed_chk_size) {
  auto remaining_size =
      chunk->Size() - (needed_chk_size + sizeof(AllocatedChunk));

  auto score = 0;
  score += chunk->Size() == needed_chk_size ? 100 : 0;
  score += IsPowerOf2(remaining_size) ? 1 : 0;

  return score;
}

[[nodiscard]] AllocatedChunk* LinkedAllocator::FindWellAlignedChunk(
    size_t needed_chk_size) const {
  struct ChunkScore {
    AllocatedChunk* chunk;
    signed int score;
  };
  ChunkScore best = {.chunk = this->arena_.head, .score = -1};

  SR_LA_LA_LOG("la/fwac#c.sz:%d b%p\n", needed_chk_size, best.chunk);
  for (auto chunk = this->arena_.head; arena_.InHeap(chunk);
       chunk = chunk->Next()) {
    if (chunk->IsUsed()) {
      SR_LA_LA_LOG("la/fwac#u %p\n", chunk);
      continue;
    }
    if (chunk->Size() < needed_chk_size) {
      SR_LA_LA_LOG("la/fwac#l %p\n", chunk);
      continue;
    }
    if (chunk->Size() == needed_chk_size) {
      SR_LA_LA_LOG("la/fwac#= %p\n", chunk);
      return chunk;
    }

    auto score = CalculateAlignScoreScore(chunk, needed_chk_size);

    if (best.score > 0 && score <= best.score) {
      continue;
    }

    SR_LA_LA_LOG("la/fwac#. %p %d\n", chunk, score);
    best = {.chunk = chunk, .score = score};
  }

  if (best.chunk->Size() < needed_chk_size) {
    SR_LA_LA_LOG("la/fwac#nf\n");
    return nullptr;
  }

  SR_LA_LA_LOG("la/fwac#r %p\n", best.chunk);

  return best.chunk;
}

void* LinkedAllocator::Allocate(uint32_t bytes) {
  size_t requested_size = RoundUpSize(bytes);

  auto chunk = FindWellAlignedChunk(requested_size);
  if (!chunk) {
    SR_LA_LA_LOG("la/a#F0x%x\n", requested_size);
    return nullptr;
  }

  SR_LA_LA_LOG("la/a#c%p:0x%x\n", chunk, requested_size);

  chunk->MarkUsed();

  if (chunk->Size() != requested_size) {  // split chunk
    auto remaining_size =
        chunk->Size() - (requested_size + sizeof(AllocatedChunk));
    chunk->Size(requested_size);

    auto next_chunk = chunk->Next();
    next_chunk->Prev(chunk);
    next_chunk->Size(remaining_size);
    next_chunk->MarkFreed();
  }

  return chunk->Data();
}

void LinkedAllocator::Deallocate(void* ptr) {
  auto ptr_in_bytes = static_cast<uint8_t*>(ptr);
  auto chunk =
      reinterpret_cast<AllocatedChunk*>(ptr_in_bytes - sizeof(AllocatedChunk));

  auto free_chunk_begin = chunk;
  if (auto prev = &chunk->Prev(); prev && prev->IsFreed()) {
    free_chunk_begin = prev;
  }

  // next is not nullable
  auto free_chunk_end = chunk;
  if (auto next = free_chunk_end->Next(); next->IsFreed()) {
    free_chunk_end = next;
  }

  auto free_chunk_size = reinterpret_cast<uint32_t>(free_chunk_end) -
                         reinterpret_cast<uint32_t>(free_chunk_begin) +
                         free_chunk_end->Size();

  auto free_chunk = reinterpret_cast<AllocatedChunk*>(free_chunk_begin);
  free_chunk->Prev(&free_chunk_begin->Prev());
  free_chunk->Size(free_chunk_size);
  free_chunk->MarkFreed();
}

void LinkedAllocator::Dump() {
  for (auto ptr = this->arena_.head; arena_.InHeap(ptr); ptr = ptr->Next()) {
    /// @todo この printf を syoch-robotics::logger を用いた実装で置換．
    printf("Chunk[%p]: Size: 0x%08x, Freed: %d\n", ptr, ptr->Size(),
           ptr->IsFreed());
  }
}

LinkedAllocator& LinkedAllocator::GetInstance() {
  return instance;
}

void LinkedAllocator::Init(void* heap_start, size_t heap_size) {
  instance.Init_(heap_start, heap_size);
}

void LinkedAllocator::Init_(void* heap_start, size_t heap_size) {
  auto head = reinterpret_cast<AllocatedChunk*>(heap_start);

  SR_LA_LA_I_LOG(
      "LinkedAllocator has being initialized with heap_start=%p "
      "heap_size=0x%x\n",
      heap_start, heap_size);

  head->Prev(nullptr);
  head->Size(heap_size - sizeof(AllocatedChunk));
  head->MarkFreed();

  arena_.head = head;
  arena_.heap_start = reinterpret_cast<uint8_t*>(heap_start);
  arena_.heap_size = heap_size;
}

void* UseLinkedAllocator::operator new(size_t size) noexcept {
  return LinkedAllocator::GetInstance().Allocate(size);
}

void UseLinkedAllocator::operator delete(void* ptr) noexcept {
  LinkedAllocator::GetInstance().Deallocate(ptr);
}

}  // namespace robotics::binary::linked_allocator

namespace robotics::binary {
LinkedAllocator LinkedAllocator::instance;
}