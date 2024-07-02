#include <MotorController.h>
#include <ikako_m3508.h>
#include <ikarashiCAN_mk2.h>
#include <mbed.h>

#include <vector>
#include <vector>
#include <vector>
#include <vector>
#include <robotics/node/node.hpp>

class IkakoRobomasNode {
  struct controller_param
  {
    float Ts = 0.001;
    float kp = 1.0;
    float ki = 0;
    float kd = 0;
    float current_limit = 5.0;
    float omega = 1 * 2 * M_PI;
  };
  static controller_param cprm;
  static MotorParams *m3508;

 public:
  robotics::Node<float> velocity;
  MotorController controller{ControlType::VELOCITY, m3508, cprm.Ts, cprm.omega};

 public:
  IkakoRobomasNode(
    IkakoM3508 &super //ikakoMDC *mdc_ = nullptr;
    //https://github.com/nnctroboticsclub/IkakoRobomasのm3という配列を指すポインタである
    //m3{aa,ggg}でいうaa,gggとかのことm3はsuperとして扱っていい
  ) {
    m3508 = super.get_motor_params();
    m3508->D = 0.0;
    m3508->J = 0.04;

    controller.set_limit(-cprm.current_limit, cprm.current_limit);
    controller.set_pid_gain(cprm.kp, cprm.ki, cprm.kd);
    controller.start();
    
    velocity.SetChangeCallback([this, super](float velo){//veloとかは引数の名前で勝手に定義してよい
      //処理をかけ
      controller.set_reference(velo);
      super.set_ref(controller.get_output());
  });
  }
};