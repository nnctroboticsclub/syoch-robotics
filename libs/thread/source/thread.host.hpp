#include <robotics/thread/thread.hpp>

#include <thread>
#include <pthread.h>

namespace robotics::system {

class Thread::Impl {
  std::thread* thread_ = nullptr;
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
    thread_ = new std::thread([args]() {
      args->function();
      delete args;
    });

    if (name_) {
      pthread_setname_np(thread_->native_handle(), name_);
    }
  }

  void SetStackSize(size_t size) { this->stack_size_ = size; }

  void SetThreadName(const char* name) { this->name_ = name; }
};

void SleepFor(std::chrono::milliseconds duration) {
  std::this_thread::sleep_for(duration);
}

}  // namespace robotics::system