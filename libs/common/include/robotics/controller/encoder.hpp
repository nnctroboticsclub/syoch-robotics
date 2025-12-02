#pragma once

#include <cstdio>

#include "packet.hpp"
#include "controller_base.hpp"

namespace controller {

template <typename T>
struct Encoder : public ControllerBase<T> {
  using ControllerBase<T>::ControllerBase;
  bool inv_;

  bool Filter(RawPacket const& packet) override {
    return packet.element_id == (0x60 | this->assigned_id_);
  }

  void Parse(RawPacket const& packet) override {
    if (packet.size() < 2) {
      printf("Invalid packet size for Encoder (id: %d)\n", this->assigned_id_);
      return;
    }

    int16_t value_ = packet[0] << 8 | packet[1];
    T value = value_ * 360.0f / 0x7FFF;

    if (this->assigned_id_ == 0) value += -98.9898 - 10 + 20 + 180;
    if (this->assigned_id_ == 1) value += 20.9955 + 5 + 180 - 180 + 180;
    if (this->assigned_id_ == 2) value += -227.0284 + 180 + 10 + 180 + 180;

    if (inv_) {
      value = value + 179;
    }

    if (value < 0) {
      value += 360;
    }

    if (value > 360) {
      value -= 360;
    }

    this->SetValue(value);
  }

  void ToggleInv(bool inv) {
    inv_ = inv;

    auto angle = this->GetValue() + 179;
    if (angle < 0) {
      angle += 360;
    }

    if (angle > 360) {
      angle -= 360;
    }

    this->SetValue(angle);
  }
};

}  // namespace controller
