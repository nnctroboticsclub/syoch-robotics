#pragma once

#include <cstdio>
#include "controller_base.hpp"
#include "../types/angle_joystick_2d.hpp"

namespace controller {

struct AngleJoystick2D : public ControllerBase<robotics::AngleStick2D> {
  using ControllerBase::ControllerBase;

  bool Filter(RawPacket const& packet) override {
    return packet.element_id == (0x40 | assigned_id_);
  }

  void Parse(RawPacket const& packet) override {
    if (packet.size() < 2) {
      printf("Invalid packet size for angle joystick (id: %d)\n", assigned_id_);
      return;
    }

    robotics::AngleStick2D value;

    value.magnitude = (float)packet[0] / 0xff;
    value.angle = (float)packet[1] / 0xff * 360.0;

    this->SetValue(value);
  }
};

}  // namespace controller