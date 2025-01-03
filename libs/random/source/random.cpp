#include <robotics/random/random.hpp>

#include <robotics/thread/thread.hpp>

namespace robotics::system::Random {

static float value_;

static void RandomThread() {
  using namespace std::chrono_literals;
  while (true) {
    value_ = 2 * value_ + impl::Entropy();
    value_ = std::fmod(value_, 1.0f);
    SleepFor(1ms);
  }
}
void Init() {
  impl::Init();

  auto thread = new Thread();
  thread->Start(RandomThread);
}

uint8_t GetByte() {
  return static_cast<uint8_t>(value_ * 255);
}
}  // namespace robotics::system::Random

#if defined(__TEST_ON_HOST__)
#include "random.mock.hpp"
#elif defined(__MBED__)
#include "random.mbed.hpp"
#elif defined(ESP_PLATFORM)
#include "random.idf.hpp"
#endif