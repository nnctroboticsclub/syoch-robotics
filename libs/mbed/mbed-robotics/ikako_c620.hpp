#pragma once

#include <array>

#include <ikako_c620.h>
#include <robotics/assembly/motor_pair.hpp>
#include <robotics/network/dcan.hpp>

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

  const float gear_ratio;

 public:
  ikakoC620RPMEncoder(ikako_c620 &mdc, float gear_ratio = 1.0f)
      : mdc_(&mdc), gear_ratio(gear_ratio) {}

  void UpdateNode() {
    if (!mdc_) return;

    this->SetValue(mdc_->get_vel(gear_ratio));
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
        rpm_encoder_(new node::ikakoC620RPMEncoder<T>(mdc, 1)) {}

  void Update() {
    encoder_->UpdateNode();
    rpm_encoder_->UpdateNode();
  }

  Node<T> &GetEncoder() override { return *encoder_; }

  node::Motor<T> &GetMotor() override { return *motor_; }

  Node<T> &GetRPMEncoder() { return *rpm_encoder_; }
};
}  // namespace robotics::assembly

namespace robotics::registry {
class ikakoC620Group {
  ::ikako_c620 motors_[8];
  ::ikako_c620_sender sender_;
  std::array<robotics::assembly::ikakoC620Pair<float>, 8> motor_nodes_;
  ikarashiCAN_mk2 *linked_ican_;

  int report_counter = 0;

  void ReportSpeed(robotics::network::DistributedCAN &can, uint8_t id);

  void ReportEncoder(robotics::network::DistributedCAN &can, uint8_t id);

 public:
  ikakoC620Group(ikarashiCAN_mk2 *can);

  void Tick();

  void ReportTo(robotics::network::DistributedCAN &can, uint8_t id);

  int Send();

  robotics::assembly::MotorPair<float> &GetNode(int index);
};
}  // namespace robotics::registry