#pragma once

#include <array>
#include <cstddef>

namespace robotics::utils {
template <typename T, size_t N>
class NoMutexLIFO {
  /**
   * 0. Initial state
   *                         N
   * <----------------------------------------------->
   * +-----+-----+-----+-----+-----+-----+-----+-----+
   * |     |     |     |     |     |     |     |     |
   * +-----+-----+-----+-----+-----+-----+-----+-----+
   *    ^                                         ^
   *    | head_                                   | tail_
   *
   * 1. After Push(0)
   *
   * +-----+-----+-----+-----+-----+-----+-----+-----+
   * |  0  |     |     |     |     |     |     |     |
   * +-----+-----+-----+-----+-----+-----+-----+-----+
   *          ^                                   ^
   *          | head_                             | tail_
   *
   * Process:
   *   buffer_[head_] = data
   *   head_ = (head_ + 1) % N
   *
   * 2. After Push(0), Push(1), ..., Push(6)
   *
   * +-----+-----+-----+-----+-----+-----+-----+-----+
   * |  0  |  1  |  2  |  3  |  4  |  5  |  6  |     |
   * +-----+-----+-----+-----+-----+-----+-----+-----+
   *                                              ^
   *                                              | head_
   *                                              | tail_
   * head_ == tail_ -> buffer is full
   *
   *
   * 3. Pop()
   *
   * +-----+-----+-----+-----+-----+-----+-----+-----+
   * |     |  1  |  2  |  3  |  4  |  5  |  6  |     |
   * +-----+-----+-----+-----+-----+-----+-----+-----+
   *    ^                                         ^
   *    | tail_                                   | head_
   *
   * Process:
   *   tail_ = (tail_ + 1) % N
   *   auto data = buffer_[tail_]
   *
   */

  std::array<T, N> buffer_ = {};
  size_t head_ = 0;
  size_t tail_ = N - 1;

 public:
  NoMutexLIFO() {
    while (!Full()) {
      Push({});
    }
    while (!Empty()) {
      Pop();
    }
  }

  bool Empty() const volatile { return (N + tail_ - head_ + 1) % N == 0; }

  bool Full() const { return head_ == tail_; }

  void Clear() {
    head_ = 0;
    tail_ = N - 1;
  }

  bool Push(T const& data) {
    if (Full()) {
      return false;
    }

    buffer_[head_] = data;
    head_ = (head_ + 1) % N;

    return true;
  }

  bool PushN(T const* data, size_t n) {
    if (Size() + n > Capacity()) {
      return false;
    }

    auto start = head_;
    auto end = (head_ + n) % N;

    if (start < end) {
      std::copy(data, data + n, buffer_.begin() + start);
    } else {
      auto first_range_lo = start;
      auto first_range_hi = N;
      auto first_range_size = first_range_hi - first_range_lo;

      auto second_range_lo = 0;

      std::copy(data, data + first_range_size,
                buffer_.begin() + first_range_lo);
      std::copy(data + first_range_size, data + n,
                buffer_.begin() + second_range_lo);
    }

    head_ = end;

    return true;
  }

  void PopNTo(size_t n, T* data) {
    if (n > Size()) {
      return;
    }

    auto start = tail_ + 1;

    if (auto end = (tail_ + n + 1) % N; start < end) {
      std::copy(buffer_.begin() + start, buffer_.begin() + end, data);
    } else {
      auto first_range_lo = start;
      auto first_range_hi = N;
      auto first_range_size = first_range_hi - first_range_lo;

      auto second_range_lo = 0;

      std::copy(buffer_.begin() + first_range_lo,
                buffer_.begin() + first_range_hi, data);
      std::copy(buffer_.begin() + second_range_lo, buffer_.begin() + end,
                data + first_range_size);
    }

    tail_ = (tail_ + n) % N;
  }

  size_t PopAllTo(T* data) {
    auto n = Size();
    PopNTo(n, data);
    return n;
  }

  T Pop() {
    if (Empty()) {
      return {};
    }

    tail_ = (tail_ + 1) % N;
    auto data = buffer_[tail_];

    return data;
  }

  void ConsumeN(size_t n) {
    if (n > Size()) {
      return;
    }

    tail_ = (tail_ + n) % N;
  }

  T& operator[](size_t index) { return buffer_[(1 + tail_ + index) % N]; }

  size_t Size() const { return (N - (tail_ - head_) - 1) % N; }

  size_t Capacity() const { return N; }
};

}  // namespace robotics::utils