#include "thread.hpp"

#include <mbed.h>

namespace robotics::system {

class Thread::Impl {
  ::rtos::Thread* thread_ = nullptr;
  size_t stack_size_ = 0;

 public:
  Impl() {}

  void Start(const ThreadFunction& function) {
    struct A {
      ThreadFunction function;
    };
    A* args = new A{function};
    thread_ = new ::rtos::Thread(osPriorityNormal, stack_size_);

    thread_->start([args]() { args->function(); });
  }

  void SetStackSize(size_t size) { this->stack_size_ = size; }
};

Thread::Thread() { impl = new Impl(); }

void Thread::Start(const ThreadFunction& function) { impl->Start(function); }

void Thread::SetStackSize(size_t size) { impl->SetStackSize(size); }

void SleepFor(std::chrono::milliseconds duration) {
  ThisThread::sleep_for(duration);
}
}  // namespace robotics::system