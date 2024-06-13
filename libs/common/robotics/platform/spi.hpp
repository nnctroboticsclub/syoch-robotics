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

#if defined(__TEST_ON_HOST__)
#include "spi.mock.hpp"
#elif defined(__MBED__)
#include "spi.mbed.hpp"
#elif defined(ESP_PLATFORM)
#include "spi.idf.hpp"
#endif