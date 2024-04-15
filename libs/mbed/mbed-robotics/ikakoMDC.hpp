#pragma once

#include <ikakoMDC.h>
#include "motor_pair.hpp"

namespace robotics::node {

template <typename T = float>
class ikakoMDCMotor : public Motor<T> {
  ikakoMDC *mdc_ = nullptr;

  void SetSpeed(T speed) override {
    if (!mdc_) return;
    if (speed > 1) speed = 1;
    if (speed < -1) speed = -1;

    mdc_->set_speed(speed * 100);
  }

 public:
  ikakoMDCMotor(ikakoMDC &mdc) : mdc_(&mdc) {}
};

template <typename T = float>
class ikakoMDCEncoder : public Node<T> {
  ikakoMDC *mdc_ = nullptr;

 public:
  ikakoMDCEncoder(ikakoMDC &mdc) : mdc_(&mdc) {}

  void UpdateNode() {
    if (!mdc_) return;
    this->SetValue((float)mdc_->get_enc() / 200.0f * 360.0f);
  }
};

}  // namespace robotics::node
namespace robotics::assembly {

template <typename T>
class ikakoMDCPair : public MotorPair<T> {
  node::ikakoMDCMotor<T> *motor_;
  node::ikakoMDCEncoder<T> *encoder_;

 public:
  ikakoMDCPair(::ikakoMDC &mdc)
      : motor_(new node::ikakoMDCMotor<T>(mdc)),
        encoder_(new node::ikakoMDCEncoder<T>(mdc)) {}

  void Update() { encoder_->UpdateNode(); }

  Node<T> &GetEncoder() override { return *encoder_; }

  node::Motor<T> &GetMotor() override { return *motor_; }
};
}  // namespace robotics::assembly
