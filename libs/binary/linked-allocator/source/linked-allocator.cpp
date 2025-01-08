#include <robotics/binary/linked-allocator.hpp>

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

[[nodiscard]] static auto CalculateAlignScoreScore(const AllocatedChunk* chunk,
                                                   uint32_t needed_chk_size) {
  auto remaining_size =
      chunk->Size() - (needed_chk_size + sizeof(AllocatedChunk));

  auto score = 0;
  score += chunk->Size() == needed_chk_size ? 10000 : 0;
  score += IsPowerOf2(remaining_size) ? 100 : 0;
  score += std::countr_zero(remaining_size);

  return score;
}

[[nodiscard]] AllocatedChunk* LinkedAllocator::FindWellAlignedChunk(
    size_t needed_chk_size) const {
  auto ptr = this->arena_.head;

  struct ChunkScore {
    AllocatedChunk* chunk;
    int score;
  };
  ChunkScore best = {.chunk = ptr, .score = 0};

  do {
    if (ptr->IsUsed())
      continue;
    if (ptr->Size() < needed_chk_size)
      continue;
    if (ptr->Size() == needed_chk_size)
      return ptr;

    if (auto score = CalculateAlignScoreScore(ptr, needed_chk_size);
        score > best.score) {
      best = {.chunk = ptr, .score = score};
    }
  } while (arena_.InHeap(ptr = ptr->Next()));

  if (best.chunk->Size() < needed_chk_size) {
    return nullptr;
  }

  return best.chunk;
}

void* LinkedAllocator::Allocate(uint32_t bytes) {
  size_t requested_size = RoundUpSize(bytes);

  auto chunk = FindWellAlignedChunk(bytes);
  if (!chunk) {
    printf("We couldnt found %d byte chunk\n", requested_size);
    return nullptr;
  }

  printf("Use Chunk[%p] as %d byte allocated\n", chunk, requested_size);

  chunk->MarkUsed();

  if (chunk->Size() > bytes) {  // split chunk
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

  printf(
      "LinkedAllocator has being initialized with heap_start=%p "
      "heap_size=0x%x\n",
      heap_start, heap_size);

  head->Prev(nullptr);
  head->Size(heap_size - sizeof(AllocatedChunk));
  head->MarkFreed();

  arena_.head = head;
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