#pragma once

#include "motor.hpp"

namespace robotics::node {
class DummyMotor : public Motor<float> {
  void SetSpeed(float) override {
    // Do nothing
  }

};
}  // namespace robotics::node