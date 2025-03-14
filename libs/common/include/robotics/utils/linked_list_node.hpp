#pragma once

#include <array>
namespace robotics::utils {
template <typename T>
class LinkedListNode {
 public:
  auto& Prev() { return *prev; }
  auto Prev(T& ptr) { prev = &ptr; }

  auto& Next() { return *next; }
  auto Next(T& ptr) { next = &ptr; }

  bool InUse() { return in_use; }
  void InUse(bool value) { in_use = value; }

  void ResetLinkNode() {
    prev = nullptr;
    next = nullptr;
  }

 public:
  bool in_use = false;
  T* prev = 0;
  T* next = 0;
};

template <typename T, std::size_t N>
class LinkedList {
 public:
  std::array<T, N> nodes;
  T* head = nullptr;
  T* tail = nullptr;

  T* NewNode() {
    for (auto& node : nodes) {
      if (!node.InUse()) {
        node.InUse(true);
        return &node;
      }
    }

    return nullptr;
  }

 public:
  class Iterator {
    T* current;

   public:
    Iterator(T* node) : current(node) {}

    T* operator*() { return current; }

    Iterator& operator++() {
      current = &current->Next();
      return *this;
    }

    bool operator!=(const Iterator& other) { return current != other.current; }
  };

 public:
  LinkedList() {
    for (auto& node : nodes) {
      node.ResetLinkNode();
    }
  }

  auto head_node() { return head; }
  auto tail_node() { return tail; }

  auto begin() { return Iterator(head); }
  auto end() { return Iterator(nullptr); }

  void Clear() {
    for (auto& node : nodes) {
      node.InUse(false);
      node.ResetLinkNode();
    }
  }

  void Remove(T* node) {
    node->InUse(false);

    if (node == head) {
      head = &node->Next();
    }

    if (node == tail) {
      tail = &node->Prev();
    }

    if (&node->Prev() != nullptr) {
      node->Prev().Next(node->Next());
    }

    if (&node->Next() != nullptr) {
      node->Next().Prev(node->Prev());
    }
  }

  T* NewBack() {
    auto new_node = NewNode();
    if (new_node == nullptr) {
      return nullptr;
    }

    if (head == nullptr) {
      head = new_node;
      tail = new_node;
      return new_node;
    }

    tail->Next(*new_node);
    new_node->Prev(*tail);
    tail = new_node;

    return new_node;
  }
};
}  // namespace robotics::utils