#pragma once

#include <chrono>
#include <functional>

namespace robotics::system {

using ThreadFunction = std::function<void()>;

class Thread {
  class Impl;

  Impl* impl = nullptr;

 public:
  Thread();

  void Start(const ThreadFunction& function);

  void SetStackSize(size_t size);
};

void SleepFor(std::chrono::milliseconds duration);

}  // namespace robotics::system
