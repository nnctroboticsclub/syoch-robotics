#pragma once

#include <robotics/timer/timer.hpp>

#include "esp_timer.h"

namespace robotics::system {
class Timer::Impl {
  int64_t timer_started_at_us = 0;

 public:
  void Start() {
    timer_started_at_us = esp_timer_get_time();
  }

  void Stop() {
    // Do nothing
  }

  void Reset() {
    Start();
  }

  std::chrono::microseconds ElapsedTime() {
    return std::chrono::microseconds(esp_timer_get_time() - timer_started_at_us);
  }
};
}  // namespace robotics::system