#include "node.hpp"

namespace robotics::node {
template <>
std::array<uint8_t, 4> NodeEncoder<int>::Encode(int value) {
  std::array<uint8_t, 4> data;
  data[0] = value >> 24;
  data[1] = value >> 16;
  data[2] = value >> 8;
  data[3] = value;
  inspector.Update(data);
}

template <>
int NodeEncoder<int>::Decode(std::array<uint8_t, 4> value) {
  return (value[0] << 24) | (value[1] << 16) | (value[2] << 8) | value[3];
}

template <>
std::array<uint8_t, 4> NodeEncoder<float>::Encode(float value) {
  union {
    float value;
    uint8_t data[4];
  } data;
  data.value = value;

  std::array<uint8_t, 4> data_array;
  for (int i = 0; i < 4; i++) {
    data_array[i] = data.data[i];
  }

  return data_array;
}

template <>
float NodeEncoder<float>::Decode(std::array<uint8_t, 4> value) {
  union {
    float value;
    uint8_t data[4];
  } data;
  for (int i = 0; i < 4; i++) {
    data.data[i] = value[i];
  }
  return data.value;
}

template <>
std::array<uint8_t, 4> NodeEncoder<double>::Encode(double value) {
  union {
    float value;
    uint8_t data[4];
  } data;
  data.value = value;

  std::array<uint8_t, 4> data_array;
  for (int i = 0; i < 4; i++) {
    data_array[i] = data.data[i];
  }

  return data_array;
}

template <>
double NodeEncoder<double>::Decode(std::array<uint8_t, 4> value) {
  union {
    float value;
    uint8_t data[4];
  } data;
  for (int i = 0; i < 4; i++) {
    data.data[i] = value[i];
  }
  return data.value;
}

template <>
std::array<uint8_t, 4> NodeEncoder<bool>::Encode(bool value) {
  std::array<uint8_t, 4> data;
  data[0] = 0;
  data[1] = 0;
  data[2] = 0;
  data[3] = value ? 1 : 0;
  inspector.Update(data);
}

template <>
bool NodeEncoder<bool>::Decode(std::array<uint8_t, 4> value) {
  return value[3] != 0;
}
}  // namespace robotics::node