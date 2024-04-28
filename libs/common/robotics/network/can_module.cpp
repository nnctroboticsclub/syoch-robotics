#include "can_module.hpp"

namespace robotics::network {

//* CANEngine
CANEngine::CANEngine(uint8_t device_id, std::shared_ptr<CANBase> can)
    : can_(can), device_id_(device_id) {

}

void CANEngine::Init() {
  can_->Init();
can_->OnRx([this](std::uint32_t id, std::vector<uint8_t> const &data) {
    for (auto const &cb : callbacks_) {
      if (cb.Acceptable(id)) {
        cb.cb(id, data);
      }
    }
  });}

void CANEngine::RegisterModule(CANModule &mod) {
  mod.OnRegister(std::shared_ptr<CANEngine>(this));
}

int CANEngine::Send(uint32_t id, std::vector<uint8_t> const &data) {
  return can_->Send(id, data);
}

void CANEngine::OnMessage(std::uint32_t mask, std::uint32_t id,
                          EventCallback::Callback const &cb) {
  callbacks_.emplace_back(EventCallback{mask, id, cb});
}

void CANEngine::OnIdle(IdleCallback const &cb) { can_->OnIdle(cb); }

int CANEngine::GetDeviceId() const { return device_id_; }

}  // namespace robotics::network