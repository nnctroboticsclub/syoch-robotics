#pragma once

#include <chrono>

namespace robotics::system {
class TimerBase {
 public:
  virtual void Start() = 0;
  virtual void Stop() = 0;
  virtual void Reset() = 0;

  virtual std::chrono::microseconds ElapsedTime() = 0;
};
}  // namespace robotics::system

#ifdef __MBED__
#include "timer.mbed.hpp"
#elif defined(ESP_PLATFORM)
#include "timer.idf.hpp"
#else
#include "timer.mock.hpp"
#endif