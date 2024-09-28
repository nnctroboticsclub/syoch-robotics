#pragma once

#include <chrono>

namespace robotics::driver {
class PWMBase {
 public:
  virtual void pulsewidth_us(int pw) = 0;
  virtual void period_us(int period) = 0;
};
}  // namespace robotics::driver

#if defined(__TEST_ON_HOST__)
#include "pwm.mock.hpp"
#elif defined(__MBED__)
#include "pwm.mbed.hpp"
#elif defined(ESP_PLATFORM)
#include "pwm.idf.hpp"
#endif