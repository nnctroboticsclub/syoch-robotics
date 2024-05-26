#pragma once

#include <thread>

#include "thread.hpp"

namespace robotics::system {
class Thread : public ThreadBase {
 private:
  std::thread thread_;

 public:
  Thread() : thread_() {}

  void Start(const ThreadFunction& function) override {
    thread_ = std::thread(function);
  }

  void SetStackSize(size_t size) override {}
};
}  // namespace robotics::system