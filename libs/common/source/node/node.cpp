#include <bit>
#include <cstdint>
#include <robotics/node/node.hpp>

namespace robotics::node {
template <>
std::array<uint8_t, 4> NodeEncoder<int>::Encode(int value) {
  std::array<uint8_t, 4> data;
  data[0] = static_cast<uint8_t>(value >> 24);
  data[1] = static_cast<uint8_t>(value >> 16);
  data[2] = static_cast<uint8_t>(value >> 8);
  data[3] = static_cast<uint8_t>(value >> 0);
  return data;
}

template <>
int NodeEncoder<int>::Decode(std::array<uint8_t, 4> value) {
  int decoded =
      (static_cast<int>(value[0]) << 24) | (static_cast<int>(value[1]) << 16) |
      (static_cast<int>(value[2]) << 8) | static_cast<int>(value[3]);
  return decoded;
}

template <>
std::array<uint8_t, 4> NodeEncoder<float>::Encode(float value) {
  const auto as_int = std::bit_cast<uint32_t>(value);

  std::array<uint8_t, 4> data_array;
  data_array[0] = static_cast<uint8_t>(as_int >> 24);
  data_array[1] = static_cast<uint8_t>(as_int >> 16);
  data_array[2] = static_cast<uint8_t>(as_int >> 8);
  data_array[3] = static_cast<uint8_t>(as_int >> 0);

  return data_array;
}

template <>
float NodeEncoder<float>::Decode(std::array<uint8_t, 4> value) {
  uint32_t as_int = (static_cast<uint32_t>(value[0]) << 24) |
                    (static_cast<uint32_t>(value[1]) << 16) |
                    (static_cast<uint32_t>(value[2]) << 8) |
                    static_cast<uint32_t>(value[3]);
  return std::bit_cast<float>(as_int);
}

template <>
std::array<uint8_t, 4> NodeEncoder<double>::Encode(double value) {
  const auto as_int = std::bit_cast<uint32_t>((float)value);
  std::array<uint8_t, 4> data_array;
  data_array[0] = static_cast<uint8_t>(as_int >> 24);
  data_array[1] = static_cast<uint8_t>(as_int >> 16);
  data_array[2] = static_cast<uint8_t>(as_int >> 8);
  data_array[3] = static_cast<uint8_t>(as_int >> 0);

  return data_array;
}

template <>
double NodeEncoder<double>::Decode(std::array<uint8_t, 4> value) {
  uint32_t as_int = (static_cast<uint32_t>(value[0]) << 24) |
                    (static_cast<uint32_t>(value[1]) << 16) |
                    (static_cast<uint32_t>(value[2]) << 8) |
                    static_cast<uint32_t>(value[3]);
  return (double)std::bit_cast<float>(as_int);
}

template <>
std::array<uint8_t, 4> NodeEncoder<bool>::Encode(bool value) {
  std::array<uint8_t, 4> data;
  data[0] = 0;
  data[1] = 0;
  data[2] = 0;
  data[3] = value ? 1 : 0;

  return data;
}

template <>
bool NodeEncoder<bool>::Decode(std::array<uint8_t, 4> value) {
  return value[3] != 0;
}

}  // namespace robotics::node