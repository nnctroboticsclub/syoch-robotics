#pragma once

#include <ikarashiCAN_mk2.h>
#include <robotics/drivers/ikakorobomas_node.hpp>

namespace robotics::drivers::ikako_robomas_bus {
using robotics::drivers::ikako_robomas_node::IkakoRobomasNode;
using robotics::drivers::ikako_robomas_node::RobomasWrapper;

class IkakoRobomasBus {
 public:
  IkakoRobomasBus(ikarashiCAN_mk2& can) : sender(&can) {}

  int Write() { return sender.write(); }

  void Read() { sender.read(); }

  template <typename Motor>
  RobomasWrapper* NewWrapper(
      Motor* motor) requires std::is_base_of<IkakoMotor, Motor>::value {
    auto node = new RobomasWrapper(motor);
    sender.set_motors(node->GetMotor()->get_motor());
    nodes.push_back(node);
    return node;
  }

  template <typename Motor>
  IkakoRobomasNode* NewNode(
      Motor* motor) requires std::is_base_of<IkakoMotor, Motor>::value {
    auto node = new IkakoRobomasNode(motor);
    sender.set_motors(node->GetMotor()->get_motor());
    nodes.push_back((RobomasWrapper*)node);
    return node;
  }

  // dt = 0.001 (1ms)
  void Update() {
    for (auto node : nodes) {
      node->Update();
    }
  }

  void Tick() { sender.read(); }

  std::vector<RobomasWrapper*> nodes;
  IkakoRobomasSender sender;
};
}  // namespace robotics::drivers::ikako_robomas_bus

namespace robotics::driver {
using drivers::ikako_robomas_bus::IkakoRobomasBus;
}