#pragma once

#include "thread.mbed.hpp"

namespace robotics::system {

class Thread::Impl {
  ::rtos::Thread thread_;

 public:
  Impl(size_t stack_size) : thread_{osPriorityNormal, stack_size} {}

  void Start(const ThreadFunction& function) {
    struct A {
      ThreadFunction function;
    };
    A* args = new A{function};

    thread_.start([args]() { args->function(); });
  }
};

Thread::Thread() {}

void Thread::Start(const ThreadFunction& function) {
  impl = new Impl(stack_size_);
  impl->Start(function);
}

void Thread::SetStackSize(size_t size) { this->stack_size_ = size; }

void SleepFor(std::chrono::milliseconds duration) {
  ThisThread::sleep_for(duration);
}
}  // namespace robotics::system