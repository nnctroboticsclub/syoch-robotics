
#if defined(__TEST_ON_HOST__)
#error "Not implemented"
#define ____INC_SYS_THREAD
#include "thread.hpp"

namespace robotics::system {
class Thread::Impl {
 private:
  std::thread thread_;

 public:
  Impl() : thread_() {}

  void Start(const ThreadFunction& function) {
    thread_ = std::thread(function);
  }

  void SetStackSize(size_t) {}
};
}  // namespace robotics::system
#elif defined(__MBED__)
#include "thread.mbed.hpp"
#elif defined(ESP_PLATFORM)
#include "thread.idf.hpp"
#else
#error "Not implemented"
#endif

#include "thread.hpp"


robotics::system::Thread::Thread() { impl_ = new robotics::system::Thread::Impl(); }

void robotics::system::Thread::Start(const robotics::system::ThreadFunction& function) { impl_->Start(function); }

void robotics::system::Thread::SetStackSize(size_t size) { impl_->SetStackSize(size); }
