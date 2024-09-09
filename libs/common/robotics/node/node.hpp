#pragma once

#include <functional>
#include <vector>

#include "node_inspector.hpp"

namespace robotics {
namespace node {

template <typename T>
class NodeEncoder {
 protected:
  NodeInspector inspector;

 public:
  NodeEncoder() : inspector(0) {}

  std::array<uint8_t, 4> Encode(T value);
  T Decode(std::array<uint8_t, 4> data);

  void Update(T value) { inspector.Update(Encode(value)); }

  void Link(NodeEncoder<T>& other_encoder) {
    inspector.Link(other_encoder.inspector);
  }
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
  Node(T value) : value_(value) {
    SetChangeCallback([this](T value) { inspector.Update(value); });
  }

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

    Propagate();
  }

  T GetValue() { return value_; }

  void SetChangeCallback(Callback callback) { callbacks_.push_back(callback); }

  void Link(Node<T>& input) {
    linked_inputs_.push_back(&input);

    inspector.Link(input.inspector);
  }

  void Propagate() {
    for (auto& input : linked_inputs_) {
      input->SetValue(value_);
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