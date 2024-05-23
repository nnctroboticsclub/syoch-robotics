#pragma once

#include <mbed.h>

#include "thread.hpp"

namespace robotics::system {

class ThreadImpl {
  ::rtos::Thread thread_;

 public:
  ThreadImpl(size_t stack_size) : thread_{osPriorityNormal, stack_size} {}

  void Start(const ThreadFunction& function) {
    struct A {
      ThreadFunction function;
    };
    A* args = new A{function};

    thread_.start([args]() { args->function(); });
  }
};

class Thread : public ThreadBase {
 private:
  ThreadImpl* impl = nullptr;

  size_t stack_size_ = 4096;

 public:
  Thread() {}

  void Start(const ThreadFunction& function) override {
    impl = new ThreadImpl(stack_size_);
    impl->Start(function);
  }

  void SetStackSize(size_t size) override { this->stack_size_ = size; }
};

void SleepFor(std::chrono::milliseconds duration) {
  ThisThread::sleep_for(duration);
}
}  // namespace robotics::system