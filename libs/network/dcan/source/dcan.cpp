#include <robotics/network/dcan.hpp>

#include <chrono>

using namespace std::chrono_literals;

namespace robotics::network {
DistributedCAN::DistributedCAN(int can_id, std::shared_ptr<CANBase> can)
    : CANEngine(can_id, can), can_(can) {}

void DistributedCAN::Init() {
  CANEngine::Init();

  this->RegisterModule(ping_pong_mod);
  this->RegisterModule(status_mod);
  this->RegisterModule(keep_alive_mod);

  //* Set status to CANReady
  SetStatus(can_module::Status::Statuses::kCANReady);
}

CANBase::Capability DistributedCAN::GetCapability() {
  return can_->GetCapability();
}

float DistributedCAN::GetBusLoad() { return can_->GetBusLoad(); }

void DistributedCAN::OnRx(CANBase::RxCallback cb) { can_->OnRx(cb); }
void DistributedCAN::OnTx(CANBase::TxCallback cb) { can_->OnTx(cb); }

void DistributedCAN::SendKeepAlive() { can_->Send(0xfc, {}); }
void DistributedCAN::OnKeepAliveLost(
    can_module::KeepAlive::KeepAliveLostCallback cb) {
  keep_alive_mod.OnKeepAliveLost(cb);
}
void DistributedCAN::OnKeepAliveRecovered(
    can_module::KeepAlive::KeepAliveRecoverdCallback cb) {
  keep_alive_mod.OnKeepAliveRecovered(cb);
}

void DistributedCAN::SetStatus(can_module::Status::Statuses status) {
  status_mod.SetStatus(status);
}

void DistributedCAN::Ping() { ping_pong_mod.Ping(); }
void DistributedCAN::OnPong(can_module::PingPong::PongListener cb) {
  ping_pong_mod.OnPong(cb);
}

}  // namespace robotics::network