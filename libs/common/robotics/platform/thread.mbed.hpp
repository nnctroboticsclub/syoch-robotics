#pragma once

#include <mbed.h>

#include "thread.hpp"

namespace robotics::system {
class Thread : public ThreadBase {
 private:
  ::rtos::Thread thread_;

 public:
  Thread() : thread_() {}

  void Start(const ThreadFunction& function) override {
    struct A{
      ThreadFunction function;
    }; A *args = new A{function};

    thread_.start([args]() { args->function(); });
  }
};

void SleepFor(std::chrono::milliseconds duration) {
  ThisThread::sleep_for(duration);
}
}  // namespace robotics::system