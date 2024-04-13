#pragma once

#include <mbed.h>

#include "timer.hpp"

namespace robotics::system {
class Timer : public TimerBase {
 private:
  ::mbed::Timer timer_;

 public:
  void Start() override { timer_.start(); }

  void Stop() override { timer_.stop(); }

  void Reset() override { timer_.reset(); }

  std::chrono::microseconds ElapsedTime() override {
    return timer_.elapsed_time();
  }
};
}  // namespace robotics::system