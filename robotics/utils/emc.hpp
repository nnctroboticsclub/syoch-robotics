#pragma once

#include <memory>
#include <mbed.h>

#include "../node/node.hpp"

namespace robotics::utils {
using EMCNode = Node<bool>;
class EMC {
 public:
  Node<bool> output;

 private:
  std::vector<std::shared_ptr<EMCNode>> emc_nodes;

  void UpdateOutput() {
    bool emc = true;
    for (auto &node : emc_nodes) {
      emc &= node->GetValue();
    }

    output.SetValue(emc);
  }

 public:
  EMC() {}

  std::shared_ptr<EMCNode> AddNode() {
    auto node = std::make_shared<EMCNode>();
    emc_nodes.push_back(node);
    node->SetChangeCallback([this](bool) { this->UpdateOutput(); });
    return node;
  }

  void Init() { output.SetValue(true); }
};

}  // namespace robotics::utils