#pragma once

#include <functional>
#include <vector>

#include <mbed.h>
#include <rtos.h>

#include <robotics/network/can_base.hpp>

namespace robotics::network {
class SimpleCAN : public CANBase {
  CAN can_;
  bool is_can_extended_ = false;
  int freqency_ = 50E3;

  bool thread_stop_ = false;
  bool thread_running_ = false;

  std::vector<RxCallback> rx_callbacks_;
  std::vector<TxCallback> tx_callbacks_;
  std::vector<IdleCallback> idle_callbacks_;

  Thread *thread_;

  void ThreadMain();

 public:
  // 1 -> success
  inline int Send(uint32_t id, std::vector<uint8_t> const &data) override {
    for (auto const &cb : tx_callbacks_) {
      cb(id, data);
    }

    CANMessage msg;
    msg.format =
        is_can_extended_ ? CANFormat::CANExtended : CANFormat::CANStandard;
    msg.id = id;
    msg.len = data.size();
    std::copy(data.begin(), data.end(), msg.data);
    return can_.write(msg);
  }

  void SetCANExtended(bool is_can_extended) {
    is_can_extended_ = is_can_extended;
  }

  SimpleCAN(PinName rx, PinName tx, int freqency = 50E3);
  ~SimpleCAN();

  void Init() override;

  void OnRx(RxCallback cb) override;
  void OnTx(TxCallback cb) override;
  void OnIdle(IdleCallback cb) override;
};
}  // namespace robotics::network