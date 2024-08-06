#pragma once

#include "motor.hpp"

#include <memory>

#include "../platform/pwm.hpp"

namespace robotics::node {
class BD621x : public Motor<float> {
  std::shared_ptr<robotics::driver::PWMBase> rin_;
  std::shared_ptr<robotics::driver::PWMBase> fin_;

  void SetSpeed(float speed) override;

 public:
  BD621x(std::shared_ptr<robotics::driver::PWMBase> rin,
         std::shared_ptr<robotics::driver::PWMBase> fin);
};
}  // namespace robotics::node