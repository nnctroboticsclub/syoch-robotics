#include "BLDC.hpp"

namespace robotics::node {

void BLDC::SetSpeed(float speed) {
  if (status != Status::Ready) return;

  // ESC ussualy doesn't support negative speed.
  if (speed < 0) speed = 0;

  if (speed > 1) speed = 1;

  pwmout_->pulsewidth_us(min_pulsewidth_ +
                        (max_pulsewidth_ - min_pulsewidth_) * (speed));
}

BLDC::BLDC(std::shared_ptr<robotics::driver::PWMBase> out, int min_pulsewidth, int max_pulsewidth)
    : pwmout_(out),
      min_pulsewidth_(min_pulsewidth),
      max_pulsewidth_(max_pulsewidth),
      status(Status::Initialized) {
  pwmout_->period_us(2000);
  pwmout_->pulsewidth_us(0);

  SetValue(0);
  factor.SetValue(0.25);
}

void BLDC::Init0() {
  if (status != Status::Initialized) return;

  pwmout_->pulsewidth_us(max_pulsewidth_);
  status = Status::ESCInit0;
}
void BLDC::Init1() {
  if (status != Status::ESCInit0) return;

  pwmout_->pulsewidth_us(min_pulsewidth_);
  status = Status::Ready;
}
}  // namespace robotics::node