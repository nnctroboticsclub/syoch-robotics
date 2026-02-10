#pragma once

#include <NanoHW/parallel.hpp>
#include <chrono>
#include <functional>

#include "NanoHW/thread.hpp"

namespace robotics::system {

using ThreadFunction = std::function<void()>;

class Thread {
  nano_hw::thread::DynThread* th = nullptr;
  const char* thread_name_{};
  int stack_size_ = 4096;

 public:
  inline Thread() = default;

  inline void Start(const ThreadFunction& function) {
    th = new nano_hw::thread::DynThread(ThreadPriorityNormal, stack_size_,
                                        nullptr, thread_name_);
    th->Start(function);
  }
  inline void SetThreadName(const char* name) { thread_name_ = name; }
  inline void SetStackSize(size_t size) { stack_size_ = size; }
};

inline void SleepFor(std::chrono::milliseconds duration) {
  nano_hw::parallel::SleepForMS(duration);
}

}  // namespace robotics::system
