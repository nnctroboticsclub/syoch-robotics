#pragma once

#include "../platform/panic.hpp"

namespace robotics::types {
template <typename T, typename E>
class Result {
  enum class Tag {
    kOk,
    kError,
  };

  Tag tag_;

  T value_;
  E error_;

 public:
  Result(T value) : tag_(Tag::kOk), value_(value) {}

  Result(E error) : tag_(Tag::kError), error_(error) {}

  ~Result() {}

  T Unwrap() const {
    if (tag_ == Tag::kError) {
      system::panic("Result is Error");
    }
    return value_;
  }

  E UnwrapError() const {
    if (tag_ == Tag::kOk) {
      system::panic("Result is Ok");
    }
    return error_;
  }

  bool IsOk() const { return tag_ == Tag::kOk; }
};

template <typename E>
class Result<void, E> {
  enum class Tag {
    kOk,
    kError,
  };

  Tag tag_;

  E error_;

 public:
  Result() : tag_(Tag::kOk) {}

  Result(E error) : tag_(Tag::kError), error_(error) {}

  ~Result() {}

  E UnwrapError() const {
    if (tag_ == Tag::kOk) {
      system::panic("Result is Ok");
    }
    return error_;
  }

  bool IsOk() const { return tag_ == Tag::kOk; }
};

}  // namespace robotics::types