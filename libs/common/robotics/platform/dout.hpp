#pragma once

#include <vector>
#include <cstdint>

namespace robotics::driver {
class IDout {
 public:
  virtual bool Write(bool value) = 0;
};
}  // namespace robotics::driver

#ifdef __MBED__
#include "dout.mbed.hpp"
#elif defined(ESP_PLATFORM)
#include "dout.idf.hpp"
#endif