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
};
}  // namespace robotics::system