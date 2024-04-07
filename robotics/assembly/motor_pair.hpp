#pragma once

#include "../node/motor.hpp"

namespace robotics::assembly {
template <typename T, typename E = T>
class MotorPair {
 public:
  virtual node::Motor<T> &GetMotor() = 0;
  virtual Node<E> &GetEncoder() = 0;
};
}  // namespace robotics::assembly