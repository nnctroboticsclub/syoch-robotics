#pragma once

#include <robotics/node/motor.hpp>

#include <NanoHW/pwm.hpp>

namespace robotics::node {

template <nano_hw::PwmOut PwmOut>
class BLDC : public Motor<float> {
  enum Status { Initialized, ESCInit0, Ready };

  PwmOut pwm;
  int min_pulsewidth_, max_pulsewidth_;
  Status status;

  void SetSpeed(float speed) override {
    if (status != Status::Ready)
      return;

    // ESC ussualy doesn't support negative speed.
    if (speed < 0)
      speed = 0;

    if (speed > 1)
      speed = 1;

    auto pulsewidth_us =
        min_pulsewidth_ + (max_pulsewidth_ - min_pulsewidth_) * (speed);
    pwm.Write(pulsewidth_us * 1e-6);
  }

 public:
  BLDC(PwmOut out, int min_pulsewidth, int max_pulsewidth)
      : pwm(out),
        min_pulsewidth_(min_pulsewidth),
        max_pulsewidth_(max_pulsewidth),
        status(Initialized) {
    pwm.SetPeriod(2000e-6);
    pwm.Write(0);

    this->SetSpeed(0);
    this->factor.SetValue(0.25);
  }

  void Init0() {
    if (status != Status::Initialized)
      return;

    pwm.Write(max_pulsewidth_ * 1e-6);
    status = Status::ESCInit0;
  }

  void Init1() {
    if (status != Status::ESCInit0)
      return;

    pwm.Write(min_pulsewidth_ * 1e-6);
    status = Status::Ready;
  }
};
}  // namespace robotics::node