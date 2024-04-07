#pragma once

#include "controller_base.hpp"
#include "../robotics/types/angle_joystick_2d.hpp"

namespace controller {

struct Action : public ControllerBase<bool> {
 private:
  std::vector<std::function<void()>> handlers;

 public:
  using ControllerBase::ControllerBase;

  bool Filter(RawPacket const& packet) override {
    return packet.element_id == (assigned_id_ | 0x80);
  }

  void Parse(RawPacket const& packet) override {
    if (packet.element_id & 0x80)
      for (auto& handler : handlers) {
        handler();
      }
  }

  void OnFire(std::function<void()> handler) { handlers.push_back(handler); }
};

}  // namespace controller