#pragma once

#include <array>
#include <ikarashiCAN_mk2.h>

#include <robotics/assembly/ikako_c620.hpp>

#include <robotics/network/dcan.hpp>

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
  ikakoC620Group(ikarashiCAN_mk2 *can, int mdc_id);

  void Tick();

  void ReportTo(robotics::network::DistributedCAN &can, uint8_t id);

  int Send();

  robotics::assembly::MotorPair<float> &GetNode(int index);
};
}  // namespace robotics::registry