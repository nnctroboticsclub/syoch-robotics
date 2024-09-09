#pragma once

#include "vector.hpp"

namespace robotics {
inline namespace types {

using JoyStick2D = Vector<float, 2>;

}  // namespace types
}  // namespace robotics

namespace robotics::node {
template <>
std::array<uint8_t, 4> NodeEncoder<JoyStick2D>::Encode(JoyStick2D value) {
  uint16_t x = value[0] * 0x7FFF + 0x7FFF;
  uint16_t y = value[1] * 0x7FFF + 0x7FFF;

  std::array<uint8_t, 4> data_array;
  data_array[0] = x >> 8;
  data_array[1] = x & 0xFF;
  data_array[2] = y >> 8;
  data_array[3] = y & 0xFF;

  return data_array;
}

template <>
JoyStick2D NodeEncoder<JoyStick2D>::Decode(std::array<uint8_t, 4> data) {
  uint16_t x = (data[0] << 8) | data[1];
  uint16_t y = (data[2] << 8) | data[3];

  return {static_cast<float>(x - 0x7FFF) / 0x7FFF,
          static_cast<float>(y - 0x7FFF) / 0x7FFF};
}

}  // namespace robotics::node