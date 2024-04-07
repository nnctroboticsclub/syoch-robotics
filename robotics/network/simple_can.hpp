#pragma once

#include <functional>
#include <vector>

#include <mbed.h>
#include <rtos.h>

#include "./can_base.hpp"

namespace robotics::network {
class SimpleCAN : public CANBase {
 public:
 private:
  CAN can_;
  int freqency_ = 50E3;

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
    msg.id = id;
    msg.len = data.size();
    std::copy(data.begin(), data.end(), msg.data);
    return can_.write(msg);
  }

  SimpleCAN(PinName rx, PinName tx, int freqency = 50E3);

  void Init() override;

  void OnRx(RxCallback cb) override;
  void OnTx(TxCallback cb) override;
  void OnIdle(IdleCallback cb) override;
};
}  // namespace robotics::network