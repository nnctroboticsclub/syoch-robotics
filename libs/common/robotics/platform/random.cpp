#include "random.hpp"

#include "thread.hpp"

namespace robotics::system {
void Random::RandomThread() {
  using namespace std::chrono_literals;
  while (1) {
    value_ *= 2;
    value_ += Entropy();
    value_ = std::fmod(value_, 1.0f);
    SleepFor(1ms);
  }
}

Random::Random() {
  Thread* thread = new Thread();
  thread->Start([this]() { RandomThread(); });
}
Random* Random::GetInstance() {
  if (!instance) {
    instance = new Random();
  }
  return instance;
}

uint8_t Random::GetByte() { return Random::GetInstance()->value_ * 255; }

Random* Random::instance = nullptr;
}  // namespace robotics::system