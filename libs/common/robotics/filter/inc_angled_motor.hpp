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

  Node<T> encoder;
  Node<T> output;

 private:
  Node<T> goal;
  AngleNormalizer<T> feedback_filter;
  AngleClamper<T> output_filter;

 public:
  IncAngledMotor() : encoder(0), output(0), goal(0) {
    encoder.Link(feedback_filter.input);
    feedback_filter.output.Link(pid.fb_);

    goal.Link(pid.goal_);

    pid.output_.Link(output_filter.input);
    output_filter.output.Link(output);
  }

  void AddAngle(T value) { goal.SetValue(goal.GetValue() + value); }

  void Update(float dt) { pid.Update(dt); }
};
}  // namespace robotics::filter