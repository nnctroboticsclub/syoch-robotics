#pragma once

#include <array>
#include "motor.hpp"

#include "../../filter/angle_smoother.hpp"
#include "../../filter/muxer.hpp"
#include "../../filter/pid.hpp"
#include "../../node/node.hpp"
#include "../../types/joystick_2d.hpp"

namespace robotics::component {
namespace swerve {
class Swerve {
 public:
  struct Config {
    std::array<float, 3> angle_offsets;
  };

  Node<float> robot_angle;  // in: feedback robot angle

  Node<types::JoyStick2D> move_ctrl;  // in: move velocity
  Node<float> angle_ctrl{0};          // in: angle rotation

  std::array<Motor*, 3> motors;  // for injection

  filter::PID<float> angle{1.0f, 0.0f, 0.0f, 0.0f};  // robot angle pid
 private:
  filter::AngleNormalizer<float> rot_in_normalizer;  // ctrl angle normalizer
  filter::AngleNormalizer<float>
      self_rot_y_normalizer;  // robot angle normalizer
  filter::Muxer<float> rot_power_muxer;

 public:
  explicit Swerve(Config& config)
      : motors{
            new Motor(config.angle_offsets[0]),
            new Motor(config.angle_offsets[1]),
            new Motor(config.angle_offsets[2]),
        } {
    // Data flow:
    // - robot_angle >> normalizer >> anglePID
    // - angle_ctrl >> normalizer >> anglePID
    // - (AnglePID, AngleCtrl) >> Angle Muxer >> motor rotation

    // MoveCtrl >> motor velocity

    robot_angle >> self_rot_y_normalizer.input;
    self_rot_y_normalizer.output >> angle.fb_;

    angle_ctrl >> rot_in_normalizer.input;
    rot_in_normalizer.output >> angle.goal_;
    rot_power_muxer.AddInput(angle.output_);
    rot_power_muxer.AddInput(angle_ctrl);

    for (auto& motor : motors) {
      move_ctrl >> motor->velocity;
      rot_power_muxer.output_ >> motor->rotation;
    }
  }

  void Update(float dt) {
    angle.Update(dt);

    for (auto motor : motors) {
      motor->Update(dt);
    }
  }

  void SetAnglePID(bool enabled) {
    if (enabled) {
      rot_power_muxer.Select(0);
    } else {
      rot_power_muxer.Select(1);
    }
  }

  void Reset() {
    for (auto motor : motors) {
      motor->Reset();
    }

    rot_in_normalizer.Reset();
    self_rot_y_normalizer.Reset();
  }

  void InverseSteerMotor(int index) { motors[index]->InverseSteerMotor(); }
};
}  // namespace swerve

using Swerve = swerve::Swerve;
}  // namespace robotics::component