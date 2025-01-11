#pragma once

namespace robotics::utils {
template <typename T>
class LinkedListNode {
 public:
  auto& Prev() { return *prev; }
  auto Prev(T& ptr) { prev = &ptr; }

  auto& Next() { return *next; }
  auto Next(T& ptr) { next = &ptr; }

 private:
  T* prev = 0;
  T* next = 0;
};
}  // namespace robotics::utils