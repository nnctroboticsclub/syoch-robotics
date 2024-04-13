#pragma once

#include <robotics/node/node.hpp>

namespace robotics::utils {

template <typename T>
class Watcher : public robotics::Node<T> {
 private:
  T value_;
  bool updated_;
  bool invalidated_ = true;

 public:
  Watcher() : value_(0), updated_(false) {}
  Watcher(T const& value) : value_(value), updated_(false) {}

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