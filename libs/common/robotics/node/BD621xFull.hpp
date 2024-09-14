#pragma once

#include "motor.hpp"

#include <memory>

#include "../platform/dout.hpp"

namespace robotics::node {
class BD621xFull : public Motor<float> {
  std::shared_ptr<robotics::driver::Dout> rin_;
  std::shared_ptr<robotics::driver::Dout> fin_;

  void SetSpeed(float speed) override;

 public:
  BD621xFull(std::shared_ptr<robotics::driver::Dout> rin,
             std::shared_ptr<robotics::driver::Dout> fin);
};
}  // namespace robotics::node