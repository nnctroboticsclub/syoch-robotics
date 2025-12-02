#pragma once

#include <cstdio>

#include "packet.hpp"
#include "../types/pid_gains.hpp"
#include "controller_base.hpp"

namespace controller {

struct PID : public ControllerBase<robotics::PIDGains> {
  using ControllerBase::ControllerBase;

  bool Filter(RawPacket const& packet) override {
    return packet.element_id == (0xA0 | assigned_id_);
  }

  void Parse(RawPacket const& packet) override {
    if (packet.size() < 3) {
      printf("Invalid packet size for PID controller (id: %d)\n", assigned_id_);
    }

    robotics::PIDGains value = this->GetValue();

    value.p = packet[0] / 255.0f * 10.0f;
    value.i = packet[1] / 255.0f * 10.0f;
    value.d = packet[2] / 255.0f * 10.0f;

    printf("PID[%d] P: %f I: %f D: %f\n", assigned_id_, value.p, value.i,
           value.d);
    this->SetValue(value);
  }
};

}  // namespace controller