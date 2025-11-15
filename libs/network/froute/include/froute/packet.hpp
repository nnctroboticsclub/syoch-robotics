#pragma once

#include <array>
#include <cstdint>

namespace robotics::network::froute {
struct Packet {
  uint8_t from;
  uint8_t goal;
  uint8_t flags;

  std::array<uint8_t, 32> data;
  uint32_t size;
};
}  // namespace robotics::network::froute