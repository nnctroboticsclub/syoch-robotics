#pragma once

#include "vector.hpp"

namespace robotics {
inline namespace types {

using JoyStick2D = Vector<float, 2>;

}  // namespace types
}  // namespace robotics

namespace robotics::node {
template <>
class NodeEncoder<JoyStick2D> : public NodeEncoder<void> {
 public:
  NodeEncoder() : NodeEncoder<void>() {}
  void Update(JoyStick2D value) {
    uint16_t x = value[0] * 0x7FFF + 0x7FFF;
    uint16_t y = value[1] * 0x7FFF + 0x7FFF;

    std::array<uint8_t, 4> data_array;
    data_array[0] = x >> 8;
    data_array[1] = x & 0xFF;
    data_array[2] = y >> 8;
    data_array[3] = y & 0xFF;

    inspector.Update(data_array);
  }
};
}  // namespace robotics::node