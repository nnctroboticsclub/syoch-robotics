#pragma once

#include <vector>

namespace robotics::filter {
template <typename T>
struct Muxer {
  std::vector<Node<T>*> inputs_;
  Node<T> output_;
  int selected_;

  Muxer() : selected_(0) {}

  void Select(size_t index) {
    if (index >= inputs_.size()) {
      return;
    }
    selected_ = index;
    output_.SetValue(inputs_[index]->GetValue());
  }

  void AddInput(Node<T>& input) {
    int i = inputs_.size();
    inputs_.push_back(&input);
    input >> [this, i](T value) {
      if (selected_ == i) {
        output_.SetValue(value);
      }
    };
  }
};
}  // namespace robotics::filter