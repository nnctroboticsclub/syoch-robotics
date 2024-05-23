#pragma once

#include <chrono>
#include <functional>

namespace robotics::system {

using ThreadFunction = std::function<void()>;

class ThreadBase {
 public:
  virtual void Start(const ThreadFunction& function) = 0;
  virtual void SetStackSize(size_t size) = 0;
};

void SleepFor(std::chrono::milliseconds duration);

}  // namespace robotics::system

#ifdef __MBED__
#include "thread.mbed.hpp"
#elif defined(ESP_PLATFORM)
#include "thread.idf.hpp"
#else
#include "thread.mock.hpp"
#endif