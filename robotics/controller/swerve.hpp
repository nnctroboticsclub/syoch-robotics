#pragma once

#include <cstdio>

#include "joystick.hpp"
#include "angle_joystick.hpp"
#include "boolean.hpp"
#include "pid.hpp"
#include "encoder.hpp"
#include "action.hpp"

#include "../robotics/component/swerve/swerve.hpp"

namespace controller::swerve {
struct SwerveController {
  struct Config {
    int joystick_id;
    int rot_right_45_id;
    int rot_left_45_id;
    int rotation_pid_enabled_id;
    int motor_0_pid_id;
    int motor_1_pid_id;
    int motor_2_pid_id;
    int angle_pid_id;
  };

  JoyStick move;
  Action rot_right_45;
  Action rot_left_45;
  Boolean rotation_pid_enabled;
  PID motor_0_pid;
  PID motor_1_pid;
  PID motor_2_pid;
  PID angle_pid;

  AngleJoystick2D angle_out;

  SwerveController(Config const& config =
                       Config{
                           .joystick_id = 0,
                           .rot_right_45_id = 0,
                           .rot_left_45_id = 1,
                           .rotation_pid_enabled_id = 1,
                           .motor_0_pid_id = 0,
                           .motor_1_pid_id = 1,
                           .motor_2_pid_id = 2,
                           .angle_pid_id = 3,
                       })
      : move(config.joystick_id),
        rot_right_45(config.rot_right_45_id),
        rot_left_45(config.rot_left_45_id),
        rotation_pid_enabled(config.rotation_pid_enabled_id),
        motor_0_pid(config.motor_0_pid_id),
        motor_1_pid(config.motor_1_pid_id),
        motor_2_pid(config.motor_2_pid_id),
        angle_pid(config.angle_pid_id),
        angle_out(0) {
    rot_left_45.OnFire([this] {
      auto angle = angle_out.GetValue();
      angle.angle -= 45;
      printf("-> %lf;\n", angle.angle);
      angle_out.SetValue(angle);
    });
    rot_right_45.OnFire([this] {
      auto angle = angle_out.GetValue();
      angle.angle += 45;
      printf("-> %lf;\n", angle.angle);
      angle_out.SetValue(angle);
    });
  }

  SwerveController(SwerveController& other) = delete;
  SwerveController operator=(SwerveController& other) = delete;

  bool Pass(RawPacket const& packet) {
    return move.Pass(packet) || rot_right_45.Pass(packet) ||
           rot_left_45.Pass(packet) || rotation_pid_enabled.Pass(packet) ||
           motor_0_pid.Pass(packet) || motor_1_pid.Pass(packet) ||
           motor_2_pid.Pass(packet) || angle_pid.Pass(packet);
  }
};

template <typename T>
struct SwerveValueStore {
  struct Config {
    int motor_0_encoder_id = 0;
    int motor_1_encoder_id = 1;
    int motor_2_encoder_id = 2;
  };

  Encoder<T> motor_0_encoder;
  Encoder<T> motor_1_encoder;
  Encoder<T> motor_2_encoder;

  SwerveValueStore(Config const& config = {})
      : motor_0_encoder(config.motor_0_encoder_id),
        motor_1_encoder(config.motor_1_encoder_id),
        motor_2_encoder(config.motor_2_encoder_id) {}

  bool Pass(RawPacket const& packet) {
    return motor_0_encoder.Pass(packet) || motor_1_encoder.Pass(packet) ||
           motor_2_encoder.Pass(packet);
  }
};

}  // namespace controller::swerve