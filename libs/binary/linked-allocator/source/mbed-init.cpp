#include <robotics/binary/linked-allocator.hpp>

extern "C" uint32_t heap_size asm("mbed_heap_size");
extern "C" uint8_t* heap_start asm("mbed_heap_start");

__attribute__((constructor)) void init_linked_allocator() {
  using robotics::binary::LinkedAllocator;

  LinkedAllocator::Init(heap_start, heap_size);
}