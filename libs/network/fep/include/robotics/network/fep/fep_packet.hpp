#pragma once

#include <cinttypes>

namespace robotics::network::fep {
struct FEPPacket {
  uint8_t from;
  uint8_t length;
  uint8_t data[64];
};
}  // namespace robotics::network::fep