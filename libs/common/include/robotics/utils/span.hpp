#pragma once

#include <concepts>
#include <cstddef>

namespace robotics::utils {
template <typename T>
class Span {
 public:
  Span(T* data, size_t size) : data_(data), size_(size) {}
  Span() : data_(nullptr), size_(0) {}

  // copy
  Span(Span<T>& other) = default;

  template <std::convertible_to<T> U>
  explicit(false) Span(const Span<U>& other)
      : data_(other.data()), size_(other.size()) {}

  T* data() const { return data_; }
  [[nodiscard]] size_t size() const { return size_; }

  T& operator[](size_t index) { return data_[index]; }

 private:
  T* data_;
  size_t size_;
};
}  // namespace robotics::utils