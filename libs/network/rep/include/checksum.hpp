#pragma once

#include <cstdint>

namespace robotics::network {
class Checksum {
  uint16_t current = 0;

 public:
  void Reset() { current = 0; }

  void operator<<(uint16_t x) {
    current = ((current & 0x007F) << 9) | ((current & 0xff80) >> 7);
    current ^= 0x35ca;
    current ^= x;
  }

  void operator<<(uint8_t x) { *this << ((uint16_t)((x << 8) | x)); }

  uint16_t Get() const { return current; }
};

}  // namespace robotics::network