#pragma once

#include <mbed.h>

#include "../node/node.hpp"

namespace robotics::node {
class DigitalOut : public Node<bool> {
 public:
  DigitalOut(PinName output) : output(output) {
    this->SetChangeCallback([this](bool value) { this->output = value; });
  }

 private:
  mbed::DigitalOut output;
};
}  // namespace robotics::node