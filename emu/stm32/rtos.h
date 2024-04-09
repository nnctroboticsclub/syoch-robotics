#pragma once

#include <thread>
#include <chrono>

namespace rtos {
namespace pseudo {

enum osPriority { osPriorityNormal };

class Thread {
  std::thread thread_;

 public:
  Thread(osPriority priority = osPriorityNormal, int stack_size = 1024 * 4);

  template <typename F>
  void start(F&& f) {
    thread_ = std::thread([f]() { f(); });
  }
};

class ThisThread {
 public:
  static void sleep_for(std::chrono::milliseconds ms);
};

void thread_sleep_for(int ms);

void wait_us(int us);
};  // namespace pseudo
using namespace pseudo;
};  // namespace rtos
using namespace rtos::pseudo;
using namespace std::chrono_literals;