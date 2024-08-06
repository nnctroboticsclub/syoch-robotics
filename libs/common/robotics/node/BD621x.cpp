#include "BD621x.hpp"

#include <mbed.h>

using namespace std::chrono_literals;

namespace robotics::node {

BD621x::BD621x(std::shared_ptr<robotics::driver::PWMBase> rin,
               std::shared_ptr<robotics::driver::PWMBase> fin)
    : rin_(rin), fin_(fin) {
  fin_->period_us(200);
  rin_->period_us(200);
  fin_->pulsewidth_us(200);
  rin_->pulsewidth_us(200);
}

void BD621x::SetSpeed(float speed) {
  if (0.05 < speed && speed < 0.05) speed = 0;

  if (speed == 0) {
    fin_->pulsewidth_us(1*200.0);
    rin_->pulsewidth_us(1*200.0);
  } else if (speed > 0) {
    fin_->pulsewidth_us(speed*200.0);
    rin_->pulsewidth_us(0*200.0);
  } else if (speed < 0) {
    fin_->pulsewidth_us(0*200.0);
    rin_->pulsewidth_us(-speed*200.0);
  }
}

}  // namespace robotics::node