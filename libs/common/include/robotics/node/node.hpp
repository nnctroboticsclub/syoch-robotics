#pragma once

#include <cstdint>

#include <functional>
#include <vector>

namespace robotics {
namespace node {

template <typename T>
class NodeEncoder {
 public:
  NodeEncoder() {}

  std::array<uint8_t, 4> Encode(T value);
  T Decode(std::array<uint8_t, 4> data);
};

class GenericNode {
 public:
  ~GenericNode();

  virtual std::array<uint8_t, 4> Encode() = 0;
  virtual void LoadFromBytes(std::array<uint8_t, 4> data) = 0;
  virtual void OnChanged(std::function<void()> data) = 0;
};

template <typename T>
class Node : public GenericNode {
 private:
  using Self = Node<T>;
  using Callback = std::function<void(T)>;
  T value_;
  std::vector<Self*> linked_inputs_;
  std::vector<Callback> callbacks_;
  NodeEncoder<T> inspector;

 public:
  Node() : Node({}) {}
  Node(T value) : value_(value) {}

  Node(Node<T>&) = delete;
  Node<T>& operator=(Node<T>&) = delete;

  void SetValue(T value, bool force_propagate = false) {
    if (value != value_) {
      value_ = value;
      Propagate(force_propagate);

      return;
    } else if (force_propagate) {
      Propagate(true);
    }
  }

  T GetValue() { return value_; }

  void SetChangeCallback(Callback callback) { callbacks_.push_back(callback); }

  void Link(Node<T>& input) { linked_inputs_.push_back(&input); }

  void Propagate(bool force_propagate) {
    for (auto& callback : callbacks_) {
      callback(value_);
    }

    for (auto& input : linked_inputs_) {
      input->SetValue(value_, force_propagate);
    }
  }

  std::array<uint8_t, 4> Encode() override { return inspector.Encode(value_); }

  void LoadFromBytes(std::array<uint8_t, 4> data) override {
    SetValue(inspector.Decode(data));
  }

  void OnChanged(std::function<void()> cb) override {
    this->SetChangeCallback([this, cb](T) { cb(); });
  }

  Node<T>& operator>>(Node<T>& next) {
    Link(next);
    return next;
  }
};
}  // namespace node

template <typename T>
using Node = node::Node<T>;

}  // namespace robotics