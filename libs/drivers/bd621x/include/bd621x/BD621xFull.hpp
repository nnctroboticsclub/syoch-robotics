#pragma once

#include <NanoHW/digital_out.hpp>
#include <robotics/node/motor.hpp>

namespace robotics::node {
template <nano_hw::DigitalOut Dout>
class BD621xFull : public Motor<float> {
  Dout rin_;
  Dout fin_;

  virtual void SetSpeed(float speed) override {
    if (-0.05 < speed && speed < 0.05)
      speed = 0;

    if (speed == 0) {
      fin_.Write(1);
      rin_.Write(1);
    } else if (speed > 0) {
      fin_.Write(1);
      rin_.Write(0);
    } else if (speed < 0) {
      fin_.Write(0);
      rin_.Write(1);
    }
  }

 public:
  BD621xFull(Dout rin, Dout fin) : rin_(rin), fin_(fin) {}
};
}  // namespace robotics::node