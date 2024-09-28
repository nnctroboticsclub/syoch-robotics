#pragma once

#include <math.h>
#include <memory>

#include "../../types/vector.hpp"
#include "../../filter/angled_motor.hpp"
#include "../../filter/joystick2angle.hpp"
#include "../../node/motor.hpp"

namespace robotics::component::swerve {

class Motor {
 public:
  Node<Vector<float, 2>> velocity;  // in: velocity
  Node<float> rotation;             // in: rotation angle

  Node<float> drive_;                 // out: drive power
  filter::AngledMotor<float> steer_;  // out: steer angle

  Node<Vector<float, 2>> vector{{0, 0}};  // debug: vector

 private:
  Vector<float, 2> normal_vector_;
  filter::Joystick2Angle angle_power;
  float angle_deg;

  void UpdateAnglePower() {
    auto rot = rotation.GetValue();
    auto vel_ = velocity.GetValue();

    Vector<float, 2> vel{
        -vel_[1],
        -vel_[0],
    };

    // auto vector_ = vel + normal_vector_ * rot / 90;
    auto vector_ =
        vel * (1 + 1.0f / 3 * rot / 90 *
                       sin(angle_deg * M_PI / 180 + atan2(vel[1], vel[0])));
    vector.SetValue(vector_);
  }

 public:
  Motor(float angle_deg) : drive_(0), angle_deg(angle_deg) {
    normal_vector_[0] = std::cos(angle_deg * M_PI / 180);
    normal_vector_[1] = std::sin(angle_deg * M_PI / 180);

    velocity.SetChangeCallback(
        [this](Vector<float, 2>) { UpdateAnglePower(); });

    rotation.SetChangeCallback([this](float) { UpdateAnglePower(); });

    vector.Link(angle_power.in);

    angle_power.out.SetChangeCallback([this](types::AngleStick2D angle_vector) {
      steer_.goal.SetValue(angle_vector.angle);
      drive_.SetValue(angle_vector.magnitude);
    });

    UpdateAnglePower();
  }

  Motor(Motor const&) = delete;
  Motor& operator=(Motor const&) = delete;

  Motor(Motor&&) = default;
  Motor& operator=(Motor&&) = default;

  void Update(float dt) { this->steer_.Update(dt); }

  void Reset() { this->steer_.Reset(); }

  void InverseSteerMotor() { steer_.AddOffset(180); }
};

}  // namespace robotics::component::swerve