#pragma once

#include <vector>
#include <cstdint>

namespace robotics::driver {
class IDout {
 public:
  virtual bool Write(bool value) = 0;
};
}  // namespace robotics::driver