#include <robotics/binary/linked-allocator.hpp>

#include <cstddef>

using robotics::binary::LinkedAllocator;

extern "C" void* malloc(size_t size) {
  auto ptr = LinkedAllocator::GetInstance().Allocate(size);

  return ptr;
}

extern "C" void free(void* ptr) {
  LinkedAllocator::GetInstance().Deallocate(ptr);
}