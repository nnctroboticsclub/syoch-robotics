#pragma once

#include <ikako_c620.h>
#include "motor_pair.hpp"

namespace robotics::node {

template <typename T = float>
class ikakoC620Motor : public Motor<T> {
  ikako_c620 *mdc_ = nullptr;

  void SetSpeed(T speed) override {
    if (!mdc_) return;
    if (speed > 1) speed = 1;
    if (speed < -1) speed = -1;

    mdc_->set(speed / 20 * 0.2);
  }

 public:
  ikakoC620Motor(ikako_c620 &mdc) : mdc_(&mdc) {}
};

template <typename T = float>
class ikakoC620Encoder : public Node<T> {
  ikako_c620 *mdc_ = nullptr;

 public:
  ikakoC620Encoder(ikako_c620 &mdc) : mdc_(&mdc) {}

  void UpdateNode() {
    if (!mdc_) return;

    this->SetValue(mdc_->get_angle());
  }
};
template <typename T = float>
class ikakoC620RPMEncoder : public Node<T> {
  ikako_c620 *mdc_ = nullptr;

 public:
  ikakoC620RPMEncoder(ikako_c620 &mdc) : mdc_(&mdc) {}

  void UpdateNode() {
    if (!mdc_) return;

    this->SetValue(mdc_->get_rpm());
  }
};

}  // namespace robotics::node
namespace robotics::assembly {

template <typename T>
class ikakoC620Pair : public MotorPair<T> {
  node::ikakoC620Motor<T> *motor_;
  node::ikakoC620Encoder<T> *encoder_;
  node::ikakoC620RPMEncoder<T> *rpm_encoder_;

 public:
  ikakoC620Pair(::ikako_c620 &mdc)
      : motor_(new node::ikakoC620Motor<T>(mdc)),
        encoder_(new node::ikakoC620Encoder<T>(mdc)),
        rpm_encoder_(new node::ikakoC620RPMEncoder<T>(mdc)) {}

  void Update() {
    encoder_->UpdateNode();
    rpm_encoder_->UpdateNode();
  }

  Node<T> &GetEncoder() override { return *encoder_; }

  node::Motor<T> &GetMotor() override { return *motor_; }

  Node<T> &GetRPMEncoder() { return *rpm_encoder_; }
};
}  // namespace robotics::assembly