#pragma once

#include <chrono>
#include <functional>

namespace robotics::system {

using ThreadFunction = std::function<void()>;

class Thread {
  class Impl;

  Impl* impl_ = nullptr;

 public:
  Thread();

  void Start(const ThreadFunction& function);
  void SetThreadName(const char* name);
  void SetStackSize(size_t size);
};

void SleepFor(std::chrono::milliseconds duration);

}  // namespace robotics::system
