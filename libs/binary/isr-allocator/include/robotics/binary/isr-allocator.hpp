#pragma once

#include <concepts>
#include <memory>

namespace robotics::binary {
template <typename T>
class ISRAllocator : public std::allocator<T> {
 public:
  using value_type = T;
  using pointer = T*;
  using const_pointer = const T*;
  using size_type = std::size_t;

  ISRAllocator() {}

  ISRAllocator(const ISRAllocator& x) {}

  template <class U>
  ISRAllocator(const ISRAllocator<U>& x) {}

  pointer allocate(size_type n, const_pointer hint = 0) { return nullptr; }

  void deallocate(pointer ptr, size_type n) {}

  template <typename U>
  struct rebind {
    typedef ISRAllocator<U> other;
  };
};
}  // namespace robotics::binary