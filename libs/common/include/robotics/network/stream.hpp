#pragma once

#include <stdint.h>

#include <functional>
#include <vector>

namespace robotics::network {
template <typename T, typename D = void, typename TxRet = void,
          typename L = uint32_t>
class Stream {
  using OnReceiveCallback = std::function<void(D, T*, L)>;

 protected:
  std::vector<OnReceiveCallback> on_receive_callbacks_;

  void DispatchOnReceive(D ctx, std::vector<T> const& data) {
    for (auto&& cb : on_receive_callbacks_) {
      if (!cb) {
        continue;
      }
      cb(ctx, data);
    }
  }
  void DispatchOnReceive(D ctx, T* data, L length) {
    for (auto&& cb : on_receive_callbacks_) {
      if (!cb) {
        continue;
      }
      cb(ctx, data, length);
    }
  }

 public:
  virtual ~Stream() = default;

  virtual TxRet Send(D ctx, T* data, L length) = 0;
  TxRet Send(D ctx, std::vector<T> const& data) {
    return Send(ctx, data.data(), data.size());
  }
  void OnReceive(OnReceiveCallback cb) {
    this->on_receive_callbacks_.emplace_back(cb);
  }
};

template <typename T, typename TxRet>
class Stream<T, void, TxRet> {
  using OnReceiveCallback = std::function<void(T* data, uint32_t length)>;

 protected:
  std::vector<OnReceiveCallback> on_receive_callbacks_;

  void DispatchOnReceive(T* data, uint32_t length) {
    for (auto&& cb : on_receive_callbacks_) {
      cb(data, length);
    }
  }

 public:
  virtual ~Stream() = default;

  virtual void Send(T* data, uint32_t length) = 0;
  TxRet Send(std::vector<T> const& data) {
    return Send(data.data(), data.size());
  }
  void OnReceive(OnReceiveCallback cb) {
    this->on_receive_callbacks_.emplace_back(cb);
  }
};

}  // namespace robotics::network