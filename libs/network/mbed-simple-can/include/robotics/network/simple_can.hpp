#pragma once

#include <vector>

#include <mbed.h>

#include <robotics/network/can_base.hpp>
#include <robotics/utils/non_copyable.hpp>

namespace robotics::network {
class SimpleCAN : public CANBase, public utils::NonCopyable<SimpleCAN> {
  class Impl;
  Impl* impl_;

 public:
  SimpleCAN(PinName rx, PinName tx, int frequency = 50E3,
            bool is_can_extended = false);
  ~SimpleCAN();

  int Send(uint32_t id, std::vector<uint8_t> const& data) override;
  void SetCANExtended(bool is_can_extended);

  void Init() override;

  void OnRx(RxCallback cb) override;
  void OnTx(TxCallback cb) override;
  void OnIdle(IdleCallback cb) override;
};
}  // namespace robotics::network