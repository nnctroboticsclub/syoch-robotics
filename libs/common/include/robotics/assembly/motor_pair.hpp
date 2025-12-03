#pragma once

#include "../node/motor.hpp"

namespace robotics::assembly {
template <typename T, typename E = T, typename MotorT = node::Motor<T>>
class MotorPair {
 public:
  virtual MotorT& GetMotor() = 0;
  virtual Node<E>& GetEncoder() = 0;
};
}  // namespace robotics::assembly