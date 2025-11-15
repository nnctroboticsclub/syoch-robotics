#pragma once

#include <memory>

#include "can_module.hpp"

#include <robotics/timer/timer.hpp>

namespace robotics::network {
class DistributedCAN : public CANEngine {
 private:
  std::shared_ptr<CANBase> can_;

  can_module::Status status_mod{};
  can_module::KeepAlive keep_alive_mod{};
  can_module::PingPong ping_pong_mod{};

 public:
  DistributedCAN(uint8_t can_id, std::shared_ptr<CANBase> can);
  ~DistributedCAN() = default;

  void Init();

  // CANBase exports
  CANBase::Capability GetCapability() const;
  float GetBusLoad() const;

  void OnRx(CANBase::RxCallback cb) const;
  void OnTx(CANBase::TxCallback cb) const;

  // KeepAlive exports
  void SendKeepAlive() const;
  void OnKeepAliveLost(can_module::KeepAlive::KeepAliveLostCallback cb);
  void OnKeepAliveRecovered(
      can_module::KeepAlive::KeepAliveRecoverdCallback cb);

  // Status exports
  void SetStatus(can_module::Status::Statuses status) const;

  // PingPong exports
  void Ping() const;
  void OnPong(can_module::PingPong::PongListener cb);
};
}  // namespace robotics::network