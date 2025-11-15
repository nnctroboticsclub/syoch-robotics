#pragma once

#include "pid.hpp"
#include "../node/node.hpp"
#include "angle_smoother.hpp"
#include "angle_clamper.hpp"

namespace robotics::filter {
template <typename T>
class IncAngledMotor {
 public:
  filter::PID<float> pid{3.0f / 360, 3.3f, 0.5f, 2.0f};

  Node<T> encoder = 0;
  Node<T> output = 0;

 private:
  Node<T> goal = 0;
  AngleNormalizer<T> feedback_filter;
  AngleClamper<T> output_filter;

 public:
  IncAngledMotor() {
    encoder.Link(feedback_filter.input);
    feedback_filter.output.Link(pid.fb_);

    goal.Link(pid.goal_);

    pid.output_ >> output_filter.input;
    output_filter.output >> output;
  }

  void AddAngle(T value) { goal.SetValue(goal.GetValue() + value); }

  void Update(float dt) { pid.Update(dt); }
};
}  // namespace robotics::filter