#pragma once

#include <mbed.h>

#include "pwm.hpp"

namespace robotics::driver {
class PWM : public PWMBase {
 private:
  mbed::PwmOut pwm_;

 public:
  PWM(PinName pin) : pwm_(pin) {}

  void pulsewidth_us(int pw) override { pwm_.pulsewidth_us(pw); }

  void period_us(int period) override { pwm_.period_us(period); }
};
}  // namespace robotics::driver