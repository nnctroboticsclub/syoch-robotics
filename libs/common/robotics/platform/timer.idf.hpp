#pragma once

#include "timer.hpp"

namespace robotics::system {
class Timer : public TimerBase {
 private:
 public:
  void Start() override {}

  void Stop() override {}

  void Reset() override {}

  std::chrono::microseconds ElapsedTime() override {
    return std::chrono::microseconds(0);
  }
};
}  // namespace robotics::system