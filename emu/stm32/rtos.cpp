#include "rtos.h"

namespace rtos {
namespace pseudo {

Thread::Thread(osPriority /* priority */, int /* stack_size */) {}

void ThisThread::sleep_for(std::chrono::milliseconds /* ms */) {
  // std::this_thread::sleep_for(ms);
}

void thread_sleep_for(int /* ms */) {
  // std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

void wait_us(int /* us */) {
  // std::this_thread::sleep_for(std::chrono::microseconds(us));
}
};  // namespace pseudo
};  // namespace rtos