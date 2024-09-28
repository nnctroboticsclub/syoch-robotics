#pragma once

#include "pwm.hpp"

namespace robotics::driver {
class PWM : public PWMBase {
 private:
 public:
  PWM()  {}

  void pulsewidth_us(int pw) override {}

  void period_us(int period) override {}
};
}  // namespace robotics::driver