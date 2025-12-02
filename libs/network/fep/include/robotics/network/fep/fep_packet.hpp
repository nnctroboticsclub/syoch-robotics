#pragma once

#include <array>
#include <cstdint>

namespace robotics::network::fep {
struct FEPPacket {
  std::uint8_t from;
  uint8_t length;
  std::array<uint8_t, 64> data;
};
}  // namespace robotics::network::fep