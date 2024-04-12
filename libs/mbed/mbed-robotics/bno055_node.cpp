#include "bno055.hpp"

namespace robotics::sensor::gyro {
void BNO055::ThreadMain() {
  bno055_.reset();

  bno055_.setpowermode(POWER_MODE_NORMAL);
  bno055_.setmode(OPERATION_MODE_NDOF);

  bno055_.set_angle_units(DEGREES);
  bno055_.set_accel_units(MPERSPERS);

  timer.start();
  while (true) {
    bno055_.get_angles();

    this->SetValue(bno055_.euler.yaw);
  }
}

BNO055::BNO055(PinName SDA, PinName SDL) : bno055_(SDA, SDL) {}

bool BNO055::Init() {
  if (!bno055_.check()) {
    return false;
  }

  thread = new rtos::Thread(osPriorityNormal, 1024 * 4);
  thread->start([this]() { ThreadMain(); });
  return true;
}
}  // namespace robotics::sensor::gyro