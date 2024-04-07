#pragma once

#include "controller_base.hpp"
#include "../robotics/types/angle_joystick_2d.hpp"

namespace controller {

struct Boolean : public ControllerBase<bool> {
  using ControllerBase::ControllerBase;

  bool Filter(RawPacket const& packet) override {
    return (packet.element_id & 0xDF) == assigned_id_;
  }

  void Parse(RawPacket const& packet) override {
    this->SetValue(packet.element_id & 0x20);
  }
};

}  // namespace controller