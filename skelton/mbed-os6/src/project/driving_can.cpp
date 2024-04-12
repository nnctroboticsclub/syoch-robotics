#include "driving_can.hpp"

namespace project {
DrivingCANBus::DrivingCANBus(ikarashiCAN_mk2 *ican) : ican_(ican) {}

void DrivingCANBus::Init() { ican_->read_start(); }

void DrivingCANBus::Tick() {}

void DrivingCANBus::Send() {}

void DrivingCANBus::ReportTo(robotics::network::DistributedCAN &can) {
  switch (report_counter) {
    case 0:
    case 1:
    case 2:
      break;
  }
  report_counter = (report_counter + 1) % 5;
}

}  // namespace project