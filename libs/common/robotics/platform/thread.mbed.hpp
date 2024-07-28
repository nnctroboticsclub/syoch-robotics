#include "thread.hpp"

#include <mbed.h>

namespace robotics::system {

class Thread::Impl {
  ::rtos::Thread* thread_ = nullptr;
  size_t stack_size_ = 4096;
  const char* name_;

 public:
  Impl() {}

  ~Impl() {
    if (thread_) delete thread_;
  }

  void Start(const ThreadFunction& function) {
    struct A {
      ThreadFunction function;
    };
    A* args = new A{function};
    thread_ = new ::rtos::Thread(osPriorityNormal, stack_size_, nullptr, name_);

    thread_->start([args]() { args->function(); });
  }

  void SetStackSize(size_t size) { this->stack_size_ = size; }

  void SetThreadName(const char* name) { this->name_ = name; }
};

void SleepFor(std::chrono::milliseconds duration) {
  ThisThread::sleep_for(duration);
}

}  // namespace robotics::system