#pragma once

#include <memory>

#include "motor.hpp"

#include "../../filter/pid.hpp"
#include "../../filter/angle_smoother.hpp"
#include "../../filter/angled_motor.hpp"
#include "../../sensor/gyro/base.hpp"
#include "../../node/node.hpp"
#include "../../filter/muxer.hpp"
#include "../../types/angle_joystick_2d.hpp"
#include "../../types/joystick_2d.hpp"
#include "../../types/vector.hpp"
#include "../../types/angle.hpp"

namespace robotics::component {
namespace swerve {
class Swerve {
 public:
  struct Config {
    float angle_offsets[3];
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
  Swerve(Config& config)
      : motors{
            new Motor(config.angle_offsets[0]),
            new Motor(config.angle_offsets[1]),
            new Motor(config.angle_offsets[2]),
        } {
    // robot_angle >> normalizer >> anglePID
    // angle_ctrl >> normalizer >> anglePID
    // (AnglePID, AngleCtrl) >> Angle Muxer >> motor rotation

    // MoveCtrl >> motor velocity

    robot_angle.Link(self_rot_y_normalizer.input);
    self_rot_y_normalizer.output.Link(angle.fb_);

    angle_ctrl.Link(rot_in_normalizer.input);
    rot_in_normalizer.output.Link(angle.goal_);

    rot_power_muxer.AddInput(angle.output_);
    rot_power_muxer.AddInput(angle_ctrl);

    for (auto& motor : motors) {
      move_ctrl.Link(motor->velocity);
      rot_power_muxer.output_.Link(motor->rotation);
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