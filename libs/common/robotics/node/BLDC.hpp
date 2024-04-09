#pragma once

#include <mbed.h>
#include "motor.hpp"

namespace robotics::node {
class BLDC : public Motor<float> {
  enum Status { Initialized, ESCInit0, Ready };

  PwmOut pwmout_;
  int min_pulsewidth_, max_pulsewidth_;
  Status status;

  void SetSpeed(float speed) override;

 public:
  BLDC(PinName pin, int min_pulsewidth, int max_pulsewidth);

  void Init0();
  void Init1();
};
}  // namespace robotics::node