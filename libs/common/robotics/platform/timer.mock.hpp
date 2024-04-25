#pragma once

#include "timer.hpp"

namespace robotics::system {
class Timer : public TimerBase {
 public:
  void Start() override {  }

  void Stop() override {  }

  void Reset() override {  }

  std::chrono::microseconds ElapsedTime() override {
    return {};
  }
};
}  // namespace robotics::system