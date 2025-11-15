#pragma once

#include "../node/node.hpp"
#include "pid.hpp"

#include "./angle_smoother.hpp"

namespace robotics::filter {
template <typename T>
class AngledMotor {
 public:
  filter::PID<float> pid{1.0f / 360, 2.0f, 0.52f, 0.13f};

  Node<float> feedback = 0;
  Node<float> goal = 0;
  Node<float> output = 0;

  Node<float> offset = 0;

 private:
  AngleNormalizer<float> feedback_normalizer;
  AngleNormalizer<float> goal_normalizer;

 public:
  AngledMotor() {
    feedback >> feedback_normalizer.input;
    feedback_normalizer.output >> pid.fb_;

    goal >> goal_normalizer.input;
    goal_normalizer.output >> pid.goal_;

    pid.output_ >> output;

    offset >> goal_normalizer.offset;
  }

  void Update(float dt) { pid.Update(dt); }

  void Reset() {
    feedback_normalizer.Reset();
    goal_normalizer.Reset();
  }

  void AddOffset(float rel_offset) {
    float new_offset = this->offset.GetValue() + rel_offset;

    if (new_offset > 180) {
      new_offset -= 360;
    } else if (new_offset < -180) {
      new_offset += 360;
    }

    this->offset.SetValue(new_offset);
  }
};
}  // namespace robotics::filter