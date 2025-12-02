#pragma once

#include <cstdint>

#include <array>
#include <functional>
#include <vector>

namespace robotics {
namespace node {

template <typename T>
class NodeEncoder {
 public:
  NodeEncoder() = default;

  std::array<uint8_t, 4> Encode(T value);
  T Decode(std::array<uint8_t, 4> data);
};

template <typename T>
struct NodeEncoderExistsType {
  using value = std::false_type;
};

template <>
struct NodeEncoderExistsType<int> {
  using value = std::true_type;
};

template <>
struct NodeEncoderExistsType<float> {
  using value = std::true_type;
};

template <>
struct NodeEncoderExistsType<double> {
  using value = std::true_type;
};

template <typename T>
using NodeEncoderExists_v = typename NodeEncoderExistsType<T>::value;

template <class T>
concept NodeEncodeExists = requires(T t) {
  {NodeEncoderExists_v<T>{}};
};

class GenericNode {
 public:
  virtual ~GenericNode() = default;

  virtual std::array<uint8_t, 4> Encode() = 0;
  virtual void LoadFromBytes(std::array<uint8_t, 4> data) = 0;
  virtual void OnChanged(std::function<void()> data) = 0;
};

template <NodeEncodeExists T>
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
  explicit(false) Node(T value) : value_(value) {}

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

  [[deprecated("Node#OnChanged is preferred over SetChangeCallback")]]
  void SetChangeCallback(Callback callback) {
    *this >> callback;
  }

  [[deprecated("pipe operator is preferred over SetChangeCallback")]]
  void OnChanged(Callback callback) {
    *this >> callback;
  }

  [[deprecated("Node#OnChanged is preferred over operator>>")]]
  void OnChanged(std::function<void()> cb) override {
    *this >> cb;
  }

  [[deprecated("Node#Link is preferred over operator>>")]]
  void Link(Node<T>& input) { linked_inputs_.push_back(&input); }

  void operator>>(Callback callback) { callbacks_.push_back(callback); }

  void operator>>(std::function<void()> cb) {
    *this >> [this, cb](T) { cb(); };
  }

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

  Node<T>& operator>>(Node<T>& next) {
    this->linked_inputs_.push_back(&next);
    return next;
  }
};
}  // namespace node

template <typename T>
using Node = node::Node<T>;

}  // namespace robotics