#pragma once

#include "packet.hpp"
#include "controller_base.hpp"

namespace controller {
struct Float : public ControllerBase<float> {
  using ControllerBase::ControllerBase;

  bool Filter(RawPacket const& packet) override {
    return packet.element_id == (0x60 | assigned_id_);
  }

  void Parse(RawPacket const& packet) override {
    if (packet.size() < 2) {
      printf("Invalid packet size for Float (id: %d)\n", assigned_id_);
      return;
    }

    float value;
    value = (packet[0] << 8 | packet[1]) / 65536.0f;

    this->SetValue(value);
  }
};

}  // namespace controller
