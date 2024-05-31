#pragma once

#include <chrono>
#include <cmath>

namespace robotics::system {
class Random {
  float value_;

  float Entropy();  // implement this function by random.xxx.hpp/cpp

  void RandomThread();

  Random();
  static Random* instance;

 public:
  static Random* GetInstance();

  static uint8_t GetByte();
};
}  // namespace robotics::system
