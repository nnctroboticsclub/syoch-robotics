#pragma once

#include <NanoHW/pwm.hpp>
#include <robotics/node/motor.hpp>

#include <NanoHW/digital_out.hpp>

#include <memory>

namespace robotics::node {
template <nano_hw::PwmOut PwmOut>
class BD621x : public Motor<float> {
  std::shared_ptr<PwmOut> rin_;
  std::shared_ptr<PwmOut> fin_;

  void SetSpeed(float speed) override {
    if (0.05 < speed && speed < 0.05)
      speed = 0;

    if (speed == 0) {
      fin_->Write(1.0);
      rin_->Write(1.0);
    } else if (speed > 0) {
      fin_->Write(1.0);
      rin_->Write(1 - speed);
    } else if (speed < 0) {
      fin_->Write(1 + speed);
      rin_->Write(1.0);
    }
  }

 public:
  BD621x(std::shared_ptr<PwmOut> rin, std::shared_ptr<PwmOut> fin)
      : rin_(rin), fin_(fin) {
    fin_->SetPeriod(50e-6);
    rin_->SetPeriod(50e-6);

    fin_->Write(1.0);
    rin_->Write(1.0);
  }
};
}  // namespace robotics::node