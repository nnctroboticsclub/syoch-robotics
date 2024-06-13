#pragma once

#include "timer.hpp"

#include <chrono>

namespace robotics::system {
class Timer::Impl {
  std::chrono::microseconds base_time_;

 public:
  void Start() {
    base_time_ = std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch());
  }

  void Stop() {}

  void Reset() { Start(); }

  std::chrono::microseconds ElapsedTime() {
    return std::chrono::duration_cast<std::chrono::microseconds>(
               std::chrono::high_resolution_clock::now().time_since_epoch()) -
           base_time_;
  }
};
}  // namespace robotics::system