#pragma once

#include <cstdio>

#include "packet.hpp"
#include "../types/joystick_2d.hpp"
#include "controller_base.hpp"

namespace controller {
struct JoyStick : public ControllerBase<robotics::JoyStick2D> {
  using ControllerBase::ControllerBase;

  bool Filter(RawPacket const& packet) override {
    return packet.element_id == (0x40 | assigned_id_);
  }

  void Parse(RawPacket const& packet) override {
    if (packet.size() < 2) {
      printf("Invalid packet size for Joystick (id: %d)\n", assigned_id_);
      return;
    }

    robotics::JoyStick2D value;
    value[0] = (packet[0] - 128) / 127.0f;
    value[1] = (packet[1] - 128) / 127.0f;

    this->SetValue(value);
  }
};

}  // namespace controller
