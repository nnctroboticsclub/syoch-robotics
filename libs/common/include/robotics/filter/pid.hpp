#pragma once

#include <robotics/node/node.hpp>
#include <robotics/types/pid_gains.hpp>

namespace robotics::filter {

template <typename T>
class PID {
 private:
  T integral_ = 0;
  T prev_error_ = 0;

 public:
  Node<PIDGains> gains;

  Node<T> fb_;
  Node<T> goal_;
  Node<T> output_;

 public:
  PID(T kG, T kP, T kI, T kD)  {
    gains.SetValue(PIDGains(kG, kP, kI, kD));
  }

  void Update(T dt) {
    PIDGains value = gains.GetValue();

    T target = goal_.GetValue();
    T feedback = fb_.GetValue();

    T error = target - feedback;
    integral_ += error * dt;
    T derivative = (error - prev_error_) / dt;
    prev_error_ = error;

    T output = value.g *
               (value.p * error + value.i * integral_ + value.d * derivative);
    output_.SetValue(output);
  }

  T CalculateError() {
    return (fb_.GetValue() - goal_.GetValue()) / goal_.GetValue();
  }
};
}  // namespace robotics::filter