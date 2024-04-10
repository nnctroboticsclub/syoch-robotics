#pragma once

#include "motor.hpp"

#include <memory>

#include "../platform/pwm.hpp"

namespace robotics::node {
class BLDC : public Motor<float> {
  enum Status { Initialized, ESCInit0, Ready };

  std::shared_ptr<robotics::driver::PWMBase> pwmout_;
  int min_pulsewidth_, max_pulsewidth_;
  Status status;

  void SetSpeed(float speed) override;

 public:
  BLDC(std::shared_ptr<robotics::driver::PWMBase> out, int min_pulsewidth, int max_pulsewidth);

  void Init0();
  void Init1();
};
}  // namespace robotics::node