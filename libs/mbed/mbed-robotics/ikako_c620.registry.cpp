#include "ikako_c620.hpp"

#if IKAKO_C620

#include <algorithm>

namespace robotics::registry {
void ikakoC620Group::ReportSpeed(robotics::network::DistributedCAN &can,
                                 uint8_t id) {
  std::vector<uint8_t> report(5);
  report.reserve(5);

  report[0] = 0x30 | id;
  report[1] =
      std::max(std::min(motor_nodes_[0].GetMotor().GetSpeed(), 1.0f), -1.0f) *
          127 +
      128;
  report[2] =
      std::max(std::min(motor_nodes_[1].GetMotor().GetSpeed(), 1.0f), -1.0f) *
          127 +
      128;
  report[3] =
      std::max(std::min(motor_nodes_[2].GetMotor().GetSpeed(), 1.0f), -1.0f) *
          127 +
      128;
  report[4] =
      std::max(std::min(motor_nodes_[3].GetMotor().GetSpeed(), 1.0f), -1.0f) *
          127 +
      128;

  auto ret = can.Send(0xa0, report);
  if (ret != 1) {
    printf("MDC Report: Sending the report is failed. (id: %d)\n", id);
  }
}

void ikakoC620Group::ReportEncoder(robotics::network::DistributedCAN &can,
                                   uint8_t id) {
  std::vector<uint8_t> report(5);
  report.reserve(5);

  report[0] = 0x50 | id;
  report[1] =
      std::max(std::min(motor_nodes_[0].GetMotor().GetSpeed(), 1.0f), -1.0f) *
          127 +
      128;
  report[2] = std::max(
      std::min(motor_nodes_[0].GetEncoder().GetValue() / 360.0f, 255.0f), 0.0f);
  report[3] = std::max(
      std::min(motor_nodes_[0].GetRPMEncoder().GetValue() / 360.0f, 255.0f),
      0.0f);
  report[4] = std::max(
      std::min(motor_nodes_[3].GetEncoder().GetValue() / 360.0f, 255.0f), 0.0f);

  auto ret = can.Send(0xa0, report);
  if (ret != 1) {
    printf("MDC Report: Sending the encoder report is failed. (id: %d)\n", id);
  }
}

ikakoC620Group::ikakoC620Group(ikarashiCAN_mk2 *can)
    : motors_{::ikako_c620(1), ::ikako_c620(2), ::ikako_c620(3),
              ::ikako_c620(4), ::ikako_c620(5), ::ikako_c620(6),
              ::ikako_c620(7), ::ikako_c620(8)},
      sender_(motors_, 8, can),
      motor_nodes_{motors_[0], motors_[1], motors_[2], motors_[3],
                   motors_[4], motors_[5], motors_[6], motors_[7]},
      linked_ican_(can) {}

void ikakoC620Group::Tick() {
  sender_.read();
  if (linked_ican_->get_read_flag()) {
    for (size_t i = 0; i < 4; i++) {
      motor_nodes_[i].Update();
    }
  }
}

void ikakoC620Group::ReportTo(robotics::network::DistributedCAN &can,
                              uint8_t id) {
  switch (report_counter) {
    case 0:
      ReportSpeed(can, id);
      break;
    case 1:
      ReportEncoder(can, id);
      break;
  }
  report_counter = (report_counter + 1) % 2;
}

int ikakoC620Group::Send() { return sender_.write(); }

robotics::assembly::MotorPair<float> &ikakoC620Group::GetNode(int index) {
  return motor_nodes_[index];
}
}  // namespace robotics::registry

#endif