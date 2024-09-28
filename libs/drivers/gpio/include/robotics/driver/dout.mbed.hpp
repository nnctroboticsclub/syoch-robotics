#pragma once

#include "dout.hpp"

#include <mbed.h>
namespace robotics::driver {
class Dout : public IDout {
 private:
  mbed::DigitalOut dout_;

 public:
  Dout(PinName pin) : dout_(pin) {}

  bool Write(bool value) override {
    dout_ = value;
    return true;
  }
};
}  // namespace robotics::driver