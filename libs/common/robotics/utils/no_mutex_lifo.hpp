#pragma once

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
    while (!Empty()) {
      Pop();
    }
  }

  bool Empty() const { return (N + tail_ - head_ + 1) % N == 0; }

  bool Full() const { return head_ == tail_; }

  bool Push(T const& data) {
    if (Full()) {
      return false;
    }

    buffer_[head_] = data;
    head_ = (head_ + 1) % N;

    return true;
  }

  T Pop() {
    if (Empty()) {
      return {};
    }

    tail_ = (tail_ + 1) % N;
    auto data = buffer_[tail_];

    return data;
  }

  T& operator[](size_t index) { return buffer_[(1 + tail_ + index) % N]; }

  size_t Size() const { return (N - (tail_ - head_) - 1) % N; }
};

}  // namespace robotics::utils