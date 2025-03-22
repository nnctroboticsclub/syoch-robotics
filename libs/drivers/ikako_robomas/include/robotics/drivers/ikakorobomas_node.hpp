#pragma once

#include <MotorController.h>
#include <ikako_m3508.h>
#include <ikarashiCAN_mk2.h>
#include <mbed.h>

#include <optional>
#include <robotics/node/node.hpp>

namespace robotics::drivers::ikako_robomas_node {
struct controller_param {
  float Ts = 0.001;
  float kp = 1.0;
  float ki = 0;
  float kd = 0;
  float current_limit = 20.0;
  float omega = 1 * 2 * M_PI;
};

class RobomasWrapper {
  controller_param cprm{};
  MotorParams* motor_params = nullptr;
  std::optional<ControlType> mode_ = std::nullopt;

  MotorController* controller = nullptr;
  IkakoMotor* super = nullptr;

  friend class IkakoRobomasNode;

 public:
  template <std::derived_from<IkakoMotor> Motor>
  RobomasWrapper(
      Motor* motor, float current_limit = 20.0f
      // https://github.com/nnctroboticsclub/IkakoRobomasのm3という配列を指すポインタである
  ) {
    super = motor;

    motor_params = super->get_motor_params();
    motor_params->D = 0.0;
    motor_params->J = 0.04;

    cprm.current_limit = current_limit;
  }

  void SetControlType(ControlType type) {
    if (controller != nullptr) {
      delete controller;
    }

    controller = new MotorController(type, motor_params, cprm.Ts, cprm.omega);
    controller->set_limit(-cprm.current_limit, cprm.current_limit);
    controller->set_pid_gain(cprm.kp, cprm.ki, cprm.kd);
    controller->start();

    mode_ = type;
  }

  void SetReference(float value) {
    if (controller == nullptr) {
      return;
    }
    controller->set_reference(value);
  }

  IkakoMotor* GetMotor() { return super; }

  bool GetReadFlag() { return super->get_read_flag(); }

  [[nodiscard]] float GetResponse() const {
    if (mode_ == std::nullopt) {
      return 0;
    }

    auto mode = *mode_;
    switch (mode) {
      case ControlType::VELOCITY: {
        return super->get_vel();
        break;
      }
      case ControlType::POSITION: {
        return super->get_angle();
        break;
      }
      default: {
        return 0;
        break;
      }
    }
  }

  // dt = 0.001 (1ms)
  void Update() {
    if (mode_ == std::nullopt) {
      super->set_ref(0);
      return;
    }
    if (controller == nullptr) {
      super->set_ref(0);
      return;
    }

    if (super->get_read_flag()) {
      controller->set_response(GetResponse());
    }

    controller->update();
    super->set_ref(controller->get_output());
  }
};

class IkakoRobomasNode : public RobomasWrapper {
 public:
  IkakoRobomasNode(IkakoMotor* motor) : RobomasWrapper(motor) {
    velocity.SetChangeCallback(
        [this](float velo) {  // veloとかは引数の名前で勝手に定義してよい
          // 処理をかけ
          controller->set_reference(velo * 50 * factor.GetValue());
        });
  }

 public:
  robotics::Node<float> velocity;
  robotics::Node<float> factor;
};

}  // namespace robotics::drivers::ikako_robomas_node

namespace robotics::driver {
using drivers::ikako_robomas_node::IkakoRobomasNode;
using drivers::ikako_robomas_node::RobomasWrapper;
}  // namespace robotics::driver