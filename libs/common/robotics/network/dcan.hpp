#pragma once

#include <unordered_map>
#include <functional>
#include <vector>
#include <iomanip>
#include <memory>

#include "can_module.hpp"

#include "../platform/timer.hpp"

namespace robotics::network {
class DistributedCAN : public CANEngine {
 private:
  std::shared_ptr<CANBase> can_;

  can_module::Status status_mod{};
  can_module::KeepAlive keep_alive_mod{};
  can_module::PingPong ping_pong_mod{};

 public:
  DistributedCAN(int can_id, std::shared_ptr<CANBase> can);
  ~DistributedCAN() = default;
  void Init();

  // CANBase exports
  CANBase::Capability GetCapability();
  float GetBusLoad();

  void OnRx(CANBase::RxCallback cb);
  void OnTx(CANBase::TxCallback cb);

  // KeepAlive exports
  void SendKeepAlive();
  void OnKeepAliveLost(can_module::KeepAlive::KeepAliveLostCallback cb);
  void OnKeepAliveRecovered(
      can_module::KeepAlive::KeepAliveRecoverdCallback cb);

  // Status exports
  void SetStatus(can_module::Status::Statuses status);

  // PingPong exports
  void Ping();
  void OnPong(can_module::PingPong::PongListener cb);
};
}  // namespace robotics::network