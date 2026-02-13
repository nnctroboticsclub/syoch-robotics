#pragma once

#include <robotics/node/motor.hpp>

#include <NanoHW/digital_out.hpp>

#include <memory>

namespace robotics::node {
template <nano_hw::DigitalOut Dout>
class BD621xFull : public Motor<float> {
  std::shared_ptr<Dout> rin_;
  std::shared_ptr<Dout> fin_;

  void SetSpeed(float speed) override {
    if (0.05 < speed && speed < 0.05)
      speed = 0;

    if (speed == 0) {
      fin_->Write(1);
      rin_->Write(1);
    } else if (speed > 0) {
      fin_->Write(1);
      rin_->Write(0);
    } else if (speed < 0) {
      fin_->Write(0);
      rin_->Write(1);
    }
  }

 public:
  BD621xFull(std::shared_ptr<Dout> rin, std::shared_ptr<Dout> fin)
      : rin_(rin), fin_(fin) {}
};
}  // namespace robotics::node