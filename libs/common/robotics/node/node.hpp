#pragma once

#include <functional>
#include <vector>

#include "node_inspector.hpp"

namespace robotics {
namespace node {

template <typename T>
class NodeEncoder {
  NodeInspector inspector;

public:
NodeEncoder(): inspector(0) {}
  void Update(std::array<uint8_t, 4> value) { inspector.Update(value); }
};

template <>
class NodeEncoder<int> {
  NodeInspector inspector;

public:
NodeEncoder(): inspector(0) {}
  void Update(int value) {
    std::array<uint8_t, 4> data;
    data[0] = value >> 24;
    data[1] = value >> 16;
    data[2] = value >> 8;
    data[3] = value;
    inspector.Update(data);
  }
};

template <>
class NodeEncoder<float> {
  NodeInspector inspector;

public:
NodeEncoder(): inspector(0) {}
  void Update(float value) {
    union {
      float value;
      uint8_t data[4];
    } data;
    data.value = value;

    std::array<uint8_t, 4> data_array;
    for (int i = 0; i < 4; i++) {
      data_array[i] = data.data[i];
    }

    inspector.Update(data_array);
  }
};

template <>
class NodeEncoder<double> {
  NodeInspector inspector;

public:
NodeEncoder(): inspector(0) {}
  void Update(double value) {
    union {
      float value;
      uint8_t data[4];
    } data;
    data.value = value;

    std::array<uint8_t, 4> data_array;
    for (int i = 0; i < 4; i++) {
      data_array[i] = data.data[i];
    }

    inspector.Update(data_array);
  }
};

template <>
class NodeEncoder<bool> {
  NodeInspector inspector;

public:
NodeEncoder(): inspector(0) {}
  void Update(bool value) {
    std::array<uint8_t, 4> data;
    data[0] = 0;
    data[1] = 0;
    data[2] = 0;
    data[3] = value ? 1 : 0;
    inspector.Update(data);
  }
};

template <typename T>
class Node {
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