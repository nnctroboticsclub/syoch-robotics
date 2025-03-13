#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <vector>

namespace robotics::network {
class CANBase {
 public:
  using RxCallback =
      std::function<void(std::uint32_t, std::vector<uint8_t> const&)>;
  using TxCallback =
      std::function<void(std::uint32_t, std::vector<uint8_t> const&)>;
  using IdleCallback = std::function<void()>;

  struct Capability {
    bool load_measure_available = false;
  };

  virtual void Init() = 0;

  virtual int Send(std::uint32_t id, std::vector<uint8_t> const& data) = 0;

  virtual void OnRx(RxCallback cb) = 0;
  virtual void OnTx(TxCallback cb) = 0;
  virtual void OnIdle(IdleCallback cb) = 0;

  virtual Capability GetCapability();
  virtual float GetBusLoad();
};

using CANRef = std::shared_ptr<CANBase>;
}  // namespace robotics::network