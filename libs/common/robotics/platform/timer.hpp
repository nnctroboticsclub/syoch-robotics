#pragma once

#include <chrono>

namespace robotics::system {
class Timer {
  class Impl;

  Impl *impl_;

 public:
  Timer();

  void Start();
  void Stop();
  void Reset();

  std::chrono::microseconds ElapsedTime();
};
}  // namespace robotics::system