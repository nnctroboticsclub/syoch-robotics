#pragma once

#include <vector>
#include <cstdint>

namespace robotics::datalink {
class ISPI {
 public:
  virtual int Transfer(std::vector<std::uint8_t> const &tx,
                        std::vector<std::uint8_t> &rx) = 0;
};
}  // namespace robotics::datalink

#ifdef __MBED__
#include "spi.mbed.hpp"
#elif defined(ESP_PLATFORM)
#include "spi.idf.hpp"
#endif