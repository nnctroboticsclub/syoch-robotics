#pragma once

#include <ikarashiCAN_mk2.h>
#include <robotics/network/dcan.hpp>

namespace project {
class DrivingCANBus {
  ikarashiCAN_mk2 *ican_;
  int report_counter = 0;

 public:
  DrivingCANBus(ikarashiCAN_mk2 *ican);

  void Init();
  void Tick();
  void Send();

  void ReportTo(robotics::network::DistributedCAN &can);
};
}  // namespace project