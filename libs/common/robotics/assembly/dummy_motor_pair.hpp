#pragma once

#include "../node/dummy_motor.hpp"
#include "motor_pair.hpp"

namespace robotics::assembly {

template <typename T>
class DummyMotorPair : public MotorPair<T> {
  node::DummyMotor *motor_;
  node::Node<T> *encoder_;

 public:
  DummyMotorPair()
      : motor_(new node::DummyMotor()), encoder_(new node::Node<T>()) {}

  Node<T> &GetEncoder() override { return *encoder_; }

  node::Motor<T> &GetMotor() override { return *motor_; }
};
}  // namespace robotics::assembly