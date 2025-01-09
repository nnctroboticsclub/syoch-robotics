#pragma once

#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstdio>

namespace robotics::binary::linked_allocator {

/// These classess is defined in source/linked-allocator.cpp
struct AllocatedChunk;

struct Arena {
  uint8_t* heap_start;
  size_t heap_size;

  AllocatedChunk* head;

  [[nodiscard]] bool InHeap(const void* ptr) const;
};

class LinkedAllocator {

  [[nodiscard]] AllocatedChunk* FindWellAlignedChunk(
      size_t needed_chk_size) const;

 public:
  void* Allocate(uint32_t bytes);
  void Deallocate(void* ptr);
  void Dump();

  //* Signleton パターン
  static LinkedAllocator& GetInstance();
  static void Init(void* heap_start, size_t heap_size);

 private:
  static LinkedAllocator instance;

 private:  //* メンバ変数
  Arena arena_;
  LinkedAllocator() = default;

  void Init_(void* heap_start, size_t heap_size);
};

class UseLinkedAllocator {
 public:
  void* operator new(size_t size) noexcept;
  void operator delete(void* ptr) noexcept;
};

}  // namespace robotics::binary::linked_allocator

namespace robotics::binary {
using linked_allocator::LinkedAllocator;
using linked_allocator::UseLinkedAllocator;
}  // namespace robotics::binary