#pragma once

#include <bno055.h>
#include <rtos.h>

#include "base.hpp"
#include "../../types/vector.hpp"

#ifndef PI
#define PI 3.14159265358979323846
#endif

namespace robotics::sensor::gyro {
class BNO055 : public Base {
  ::BNO055 bno055_;
  rtos::Thread *thread;

  Timer timer;

  void ThreadMain();

 public:
  BNO055(PinName SDA, PinName SDL);

  bool Init();
};
}  // namespace robotics::sensor::gyro