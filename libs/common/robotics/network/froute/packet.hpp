#pragma once

#include <cstdint>

namespace robotics::network::froute {
struct Packet {
  uint8_t from;
  uint8_t goal;
  uint8_t flags;

  uint8_t data[32];
  uint32_t size;
};
}  // namespace robotics::network::froute