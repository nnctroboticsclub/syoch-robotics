#include "dcan.hpp"

#include <chrono>

using namespace std::chrono_literals;

namespace robotics::network {
inline void DistributedCAN::HandleMessage(std::uint32_t id,
                                          std::vector<uint8_t> const &data) {
  for (auto const &cb : callbacks_) {
    if (cb.element_id == id) {
      cb.cb(data);
    }
  }
}

DistributedCAN::DistributedCAN(int can_id, std::shared_ptr<CANBase> can)
    : can_id(can_id), can_(can) {
  keep_alive_timer.Start();
}

void DistributedCAN::Init() {
  can_->Init();
  can_->OnRx([this](std::uint32_t id, std::vector<uint8_t> const &data) {
    this->HandleMessage(id, data);
  });

  //* Ping
  OnMessage(0x80,
            [this](std::vector<uint8_t>) { can_->Send(0x81 + can_id, {}); });

  for (uint8_t device = 0; device < 15; device++) {
    OnMessage(0x81 + device, [this, device](std::vector<uint8_t> const &data) {
      for (auto &cb : pong_listeners_) {
        cb(device);
      }
    });
  }

  //* Keep alive
  OnMessage(0xfc, [this](std::vector<uint8_t>) {
    // printf("Keepalive!\n");
    keep_alive_timer.Reset();
  });
  can_->OnIdle([this]() {
    auto timer = keep_alive_timer.ElapsedTime();
    if (keep_alive_available && 300ms < timer) {
      printf("Keepalive Lost!\n");
      keep_alive_available = false;
      for (auto &&cb : this->keep_alive_lost_callbacks_) {
        cb();
      }
    } else if (!keep_alive_available && timer < 300ms) {
      printf("Keepalive Get!\n");
      keep_alive_available = true;
      for (auto &&cb : this->keep_alive_recovered_callbacks_) {
        cb();
      }
    }
  });

  //* Set status to CANReady
  SetStatus(Statuses::kCANReady);
}

void DistributedCAN::OnMessage(uint8_t element_id,
                               std::function<void(std::vector<uint8_t>)> cb) {
  callbacks_.emplace_back(EventCallback{element_id, cb});
}

CANBase::Capability DistributedCAN::GetCapability() {
  return can_->GetCapability();
}

float DistributedCAN::GetBusLoad() { return can_->GetBusLoad(); }

void DistributedCAN::OnRx(CANBase::RxCallback cb) { can_->OnRx(cb); }
void DistributedCAN::OnTx(CANBase::TxCallback cb) { can_->OnTx(cb); }
void DistributedCAN::OnIdle(CANBase::IdleCallback cb) { can_->OnIdle(cb); }

void DistributedCAN::SendKeepAlive() { can_->Send(0xfc, {}); }
void DistributedCAN::OnKeepAliveLost(KeepAliveLostCallback cb) {
  this->keep_alive_lost_callbacks_.emplace_back(cb);
}
void DistributedCAN::OnKeepAliveRecovered(KeepAliveRecoverdCallback cb) {
  this->keep_alive_recovered_callbacks_.emplace_back(cb);
}

int DistributedCAN::Send(uint8_t element_id, std::vector<uint8_t> const &data) {
  return can_->Send(element_id, data);
}

void DistributedCAN::SetStatus(Statuses status) {
  std::vector<uint8_t> payload(4);
  payload[0] = 0xf0;
  payload[1] = (can_id >> 0x08) && 0xff;
  payload[2] = (can_id >> 0x00) & 0xff;
  payload[3] = static_cast<uint8_t>(status);

  can_->Send(0xa0, payload);
}

void DistributedCAN::Ping() { can_->Send(0x80, {}); }
void DistributedCAN::OnPong(PongListener cb) {
  pong_listeners_.emplace_back(cb);
}

}  // namespace robotics::network