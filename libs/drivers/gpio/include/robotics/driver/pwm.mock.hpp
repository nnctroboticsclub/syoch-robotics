#pragma once

#include <cstdio>

#include "pwm.hpp"

namespace robotics::driver {
class PWM : public PWMBase {
 private:
  float period_us_;
  int pin;

 public:
  PWM(int pin): pin(pin) {}

  void pulsewidth_us(int pw) override {
    float duty = static_cast<float>(pw) / period_us_;

    printf("PWM::pulsewidth_us(%d) duty: %f\n", pin, duty);
  }

  void period_us(int period) override { period_us_ = period; }
};
}  // namespace robotics::driver