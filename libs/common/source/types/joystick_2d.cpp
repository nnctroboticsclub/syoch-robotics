#include <robotics/node/node.hpp>
#include <robotics/types/joystick_2d.hpp>

namespace robotics::node {
template <>
std::array<uint8_t, 4> NodeEncoder<JoyStick2D>::Encode(JoyStick2D value) {
  auto x = static_cast<uint16_t>(value[0] * 0x7FFF + 0x7FFF);
  auto y = static_cast<uint16_t>(value[1] * 0x7FFF + 0x7FFF);

  std::array<uint8_t, 4> data_array;
  data_array[0] = x >> 8;
  data_array[1] = x & 0xFF;
  data_array[2] = y >> 8;
  data_array[3] = y & 0xFF;

  return data_array;
}

template <>
JoyStick2D NodeEncoder<JoyStick2D>::Decode(std::array<uint8_t, 4> data) {
  auto x =
      (static_cast<uint16_t>(data[0]) << 8) | static_cast<uint16_t>(data[1]);
  auto y =
      (static_cast<uint16_t>(data[2]) << 8) | static_cast<uint16_t>(data[3]);

  return {static_cast<float>(x - 0x7FFF) / 0x7FFF,
          static_cast<float>(y - 0x7FFF) / 0x7FFF};
}
}  // namespace robotics::node