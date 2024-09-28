#pragma once

#include <robotics/node/motor.hpp>
#include <robotics/driver/dout.hpp>

#include <memory>

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