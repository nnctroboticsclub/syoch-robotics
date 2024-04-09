#pragma once

#include <cstdint>

namespace robotics::network {
class CANBase {
 public:
  using RxCallback =
      std::function<void(std::uint32_t, std::vector<uint8_t> const &)>;
  using TxCallback =
      std::function<void(std::uint32_t, std::vector<uint8_t> const &)>;
  using IdleCallback = std::function<void()>;

  virtual void Init() = 0;

  virtual int Send(std::uint32_t id, std::vector<uint8_t> const &data) = 0;

  virtual void OnRx(RxCallback cb) = 0;
  virtual void OnTx(TxCallback cb) = 0;
  virtual void OnIdle(IdleCallback cb) = 0;
};
}  // namespace robotics::network