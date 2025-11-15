#pragma once

#include <robotics/node/node.hpp>

namespace robotics::utils {

template <typename T>
class Watcher : public robotics::Node<T> {
 private:
  T value_;
  bool updated_ = false;
  bool invalidated_ = true;

 public:
  Watcher() : value_(0) {}
  explicit Watcher(T const& value) : value_(value) {}

  bool Update(T const& value) {
    if (invalidated_) {
      invalidated_ = false;
      value_ = value;
      updated_ = true;
    } else if (value != value_) {
      value_ = value;
      updated_ = true;
    } else {
      updated_ = false;
    }

    if (updated_) {
      this->SetValue(value_);
    }
    return updated_;
  }
};
}