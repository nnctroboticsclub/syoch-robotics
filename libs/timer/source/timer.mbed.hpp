#pragma once

#include <mbed.h>

#include <robotics/timer/timer.hpp>
namespace robotics::system {
class Timer::Impl {
 private:
  ::mbed::Timer timer_;

 public:
  void Start() { timer_.start(); }

  void Stop() { timer_.stop(); }

  void Reset() { timer_.reset(); }

  std::chrono::microseconds ElapsedTime() { return timer_.elapsed_time(); }
};
}  // namespace robotics::system