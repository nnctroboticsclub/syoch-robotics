#pragma once

#include "../platform/dout.hpp"

#include "../node/node.hpp"

namespace robotics::node {
class DigitalOut : public Node<bool> {
 public:
  DigitalOut(std::shared_ptr<robotics::driver::IDout> out) : out_(out) {
    this->SetChangeCallback([this](bool value) { out_->Write(value); });
  }

 private:
  std::shared_ptr<robotics::driver::IDout>out_;
};
}  // namespace robotics::node