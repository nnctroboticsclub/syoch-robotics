#pragma once

#include <coroutine>
#include <functional>
#include <optional>
#include <queue>
#include <vector>

namespace robotics::network {
template <typename T>
class SyncStream {
  std::queue<T> buffer_;

  std::optional<std::function<void(T&)>> change_callback_;

 protected:
  void FeedRxData(T data) {
    buffer_.push(data);

    if (change_callback_.has_value()) {
      change_callback_.value()(buffer_.back());
    }
  }

 public:
  SyncStream() = default;
  virtual ~SyncStream() = default;

  /// @brief ストリームにデータを送信する
  virtual void Send(T data) = 0;

  /// @brief ストリームバッファにデータがある場合は受信する
  std::optional<T> Receive() {
    if (buffer_.empty()) {
      return std::nullopt;
    }

    auto ret = buffer_.front();
    buffer_.pop();
    return ret;
  }

  /// @brief 非同期でデータを受信するための awaiter
  struct RecvAwaiter {
    T value_;
    SyncStream<T>& st_;

    explicit RecvAwaiter(SyncStream<T>& st) : st_(st) {}

    [[nodiscard]] auto await_ready() const -> bool { return false; }

    auto await_suspend(std::coroutine_handle<> handle) -> void {
      st_.change_callback_ = [handle, this](T& value) {
        this->value_ = value;
        handle.resume();
      };
    }

    auto await_resume() -> T { return value_; }
  };

  /// @brief 非同期でデータを受信する
  auto ReceiveAsync() -> RecvAwaiter { return RecvAwaiter(*this); }

  /// @brief コンパイル時にデータ長さが決まっている配列を受信する
  template <size_t N>
  std::array<T, N> ReceiveArray() {
    std::array<T, N> ret;
    for (size_t i = 0; i < N; i++) {
      ret[i] = co_await ReceiveAsync();
    }
    co_return ret;
  }

  /// @brief コンパイル時にデータ長さが決まっていない配列を受信する
  std::vector<T> ReceiveN(size_t n) {
    std::vector<T> ret;
    for (size_t i = 0; i < n; i++) {
      ret.push_back(co_await ReceiveAsync());
    }
    co_return ret;
  }
};
}  // namespace robotics::network