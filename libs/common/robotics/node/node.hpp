#pragma once

#include <functional>
#include <vector>

namespace robotics {
namespace node {
template <typename T>
class Node {
 private:
  using Self = Node<T>;
  using Callback = std::function<void(T)>;
  T value_;
  std::vector<Self*> linked_inputs_;
  std::vector<Callback> callbacks_;

 public:
  Node() : Node({}) {}
  Node(T value) : value_(value) {}

  Node(Node<T>&) = delete;
  Node<T>& operator=(Node<T>&) = delete;

  void SetValue(T value) {
    if (value_ == value) {
      return;
    }

    value_ = value;

    for (auto& callback : callbacks_) {
      callback(value);
    }

    for (auto& input : linked_inputs_) {
      input->SetValue(value);
    }
  }

  T GetValue() { return value_; }

  void SetChangeCallback(Callback callback) { callbacks_.push_back(callback); }
  void Link(Node<T>& input) { linked_inputs_.push_back(&input); }

  Node<T>& operator>>(Node<T>& next) {
    Link(next);
    return next;
  }
};
}  // namespace node

template <typename T>
using Node = node::Node<T>;

}  // namespace robotics