#pragma once

#include <cstdint>

#include <functional>
#include <vector>

namespace robotics::network {
template <typename T, typename D = void, typename TxRet = void>
class BlockStream {
  using OnReceiveCallback = std::function<void(D, T&)>;

 protected:
  std::vector<OnReceiveCallback> on_receive_callbacks_;

  void DispatchOnReceive(D ctx, T& data) {
    for (auto&& cb : on_receive_callbacks_) {
      if (!cb) {
        continue;
      }
      cb(ctx, data);
    }
  }

 public:
  virtual TxRet Send(D ctx, T& data) = 0;
  void OnReceive(OnReceiveCallback cb) {
    this->on_receive_callbacks_.emplace_back(cb);
  }
};

template <typename T, typename TxRet>
class BlockStream<T, void, TxRet> {
  using OnReceiveCallback = std::function<void(T& data)>;

 protected:
  std::vector<OnReceiveCallback> on_receive_callbacks_;

  void DispatchOnReceive(T& data) {
    for (auto&& cb : on_receive_callbacks_) {
      cb(data);
    }
  }

 public:
  virtual void Send(T& data) = 0;

  void OnReceive(OnReceiveCallback cb) {
    this->on_receive_callbacks_.emplace_back(cb);
  }
};

}  // namespace robotics::network