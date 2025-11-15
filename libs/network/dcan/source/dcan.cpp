#include <robotics/network/dcan.hpp>

using namespace std::chrono_literals;

namespace robotics::network {
DistributedCAN::DistributedCAN(uint8_t can_id, std::shared_ptr<CANBase> can)
    : CANEngine(can_id, can), can_(can) {}

void DistributedCAN::Init() {
  CANEngine::Init();

  this->RegisterModule(ping_pong_mod);
  this->RegisterModule(status_mod);
  this->RegisterModule(keep_alive_mod);

  //* Set status to CANReady
  SetStatus(can_module::Status::Statuses::kCANReady);
}

CANBase::Capability DistributedCAN::GetCapability() const {
  return can_->GetCapability();
}

float DistributedCAN::GetBusLoad() const {
  return can_->GetBusLoad();
}

void DistributedCAN::OnRx(CANBase::RxCallback cb) const {
  can_->OnRx(cb);
}
void DistributedCAN::OnTx(CANBase::TxCallback cb) const {
  can_->OnTx(cb);
}

void DistributedCAN::SendKeepAlive() const {
  can_->Send(0xfc, {});
}
void DistributedCAN::OnKeepAliveLost(
    can_module::KeepAlive::KeepAliveLostCallback cb) {
  keep_alive_mod.OnKeepAliveLost(cb);
}
void DistributedCAN::OnKeepAliveRecovered(
    can_module::KeepAlive::KeepAliveRecoverdCallback cb) {
  keep_alive_mod.OnKeepAliveRecovered(cb);
}

void DistributedCAN::SetStatus(can_module::Status::Statuses status) const {
  status_mod.UpdateStatus(status);
}

void DistributedCAN::Ping() const {
  ping_pong_mod.Ping();
}
void DistributedCAN::OnPong(can_module::PingPong::PongListener cb) {
  ping_pong_mod.OnPong(cb);
}

}  // namespace robotics::network