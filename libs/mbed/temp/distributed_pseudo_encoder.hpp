#pragma once

#include "pseudo_abs_encoder.hpp"

#include <robotics/utils/watcher.hpp>
#include <robotics/network/dcan.hpp>

namespace robotics::mbed {
class DistributedPseudoAbsEncoder : public PseudoAbsEncoder {
 private:
  robotics::utils::Watcher<double> angle_;
  robotics::utils::Watcher<bool> abs_ready_detector_;

  struct {
    bool enabled;
    robotics::network::DistributedCAN* can;
    int dev_id;
  } attached_can_;

  void Send_() {
    if (!attached_can_.enabled) return;

    uint16_t value = (int16_t)(this->GetAngle() / 360.0f * 0x7FFF);

    std::vector<uint8_t> payload;
    payload.push_back(0x60 | attached_can_.dev_id);
    payload.push_back(value >> 8);
    payload.push_back(value & 0xFF);

    auto ret = attached_can_.can->Send(0x61, payload);
    if (ret != 1) {
      printf("DistributedPseudoAbsEncoder::Send_/CanSend() failed\n");
    }
  }

 public:
  using PseudoAbsEncoder::PseudoAbsEncoder;

  DistributedPseudoAbsEncoder(PinName A, PinName B, PinName index)
      : PseudoAbsEncoder(A, B, index) {
    this->abs_ready_detector_.SetChangeCallback([this](bool const& value) {
      if (!value) return;

      printf("Encoder[%d] ready!\n", this->attached_can_.dev_id);

      this->angle_.SetChangeCallback([this](double const&) { Send_(); });
    });
  }

  void AttachSend(robotics::network::DistributedCAN& can, int dev_id) {
    attached_can_.enabled = true;
    attached_can_.can = &can;
    attached_can_.dev_id = dev_id;
  }
};
}  // namespace robotics::mbed