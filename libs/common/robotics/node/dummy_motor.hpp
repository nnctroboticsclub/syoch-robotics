#pragma once

#include "motor.hpp"

namespace robotics::node {
class DummyMotor : public Motor<float> {
  void SetSpeed(float speed) override {}

 public:
};
}  // namespace robotics::node