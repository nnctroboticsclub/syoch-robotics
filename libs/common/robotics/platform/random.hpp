#pragma once

#include <chrono>
#include <cmath>

#include "thread.hpp"

namespace robotics::system {
class Random {
  float value_;

  float Entropy();  // implement this function by random.xxx.hpp/cpp

  void RandomThread() {
    using namespace std::chrono_literals;
    while (1) {
      value_ *= 2;
      value_ += Entropy();
      value_ = std::fmod(value_, 1.0f);
      SleepFor(1ms);
    }
  }

  Random() {
    Thread* thread = new Thread();
    thread->Start([this]() { RandomThread(); });
  }
  static inline Random* instance;

 public:
  static Random* GetInstance() {
    if (!instance) {
      instance = new Random();
    }
    return instance;
  }

  static uint8_t GetByte() { return Random::GetInstance()->value_ * 255; }
};
}  // namespace robotics::system

#ifdef __MBED__
#include "random.mbed.hpp"
#elif defined(ESP_PLATFORM)
#include "random.idf.hpp"
#else
#include "random.mock.hpp"
#endif