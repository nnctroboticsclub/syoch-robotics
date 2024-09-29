#pragma once

#include <ikarashiCAN_mk2.h>
#include <robotics/drivers/ikakorobomas_node.hpp>

namespace nhk2024b::common {
class IkakoRobomasBus {
 public:
  IkakoRobomasBus(ikarashiCAN_mk2 &can) : sender(&can) {}

  int Write() { return sender.write(); }

  void Read() { sender.read(); }

  template <
      typename Motor,
      std::enable_if_t<std::is_base_of<IkakoMotor, Motor>::value, bool> = true>
  IkakoRobomasNode *NewNode(int index, Motor *motor) {
    auto node = new IkakoRobomasNode(index, motor);
    sender.set_motors(node->GetMotor()->get_motor());
    nodes.push_back(node);
    return node;
  }

  // dt = 0.001 (1ms)
  void Update() {
    for (auto node : nodes) {
      node->Update();
    }
  }

  void Tick() { sender.read(); }

  std::vector<IkakoRobomasNode *> nodes;
  IkakoRobomasSender sender;
};
}  // namespace nhk2024b::common