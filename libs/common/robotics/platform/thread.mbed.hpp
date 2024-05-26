#pragma once

#include <mbed.h>

#include "thread.hpp"

namespace robotics::system {

class Thread : public ThreadBase {
  class Impl;

  Impl* impl = nullptr;
  size_t stack_size_ = 4096;

 public:
  Thread();

  void Start(const ThreadFunction& function) override;

  void SetStackSize(size_t size) override;
};

void SleepFor(std::chrono::milliseconds duration);
}  // namespace robotics::system