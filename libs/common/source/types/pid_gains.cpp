#include <cstdint>
#include <robotics/node/node.hpp>
#include <robotics/types/pid_gains.hpp>

namespace robotics::node {

template <>
std::array<uint8_t, 4> NodeEncoder<PIDGains>::Encode(PIDGains value) {
  std::array<uint8_t, 4> data;

  data[0] = static_cast<uint8_t>(value.g / 2.0 * 255.0f);
  data[1] = static_cast<uint8_t>(value.p / 2.0 * 255.0f);
  data[2] = static_cast<uint8_t>(value.i / 2.0 * 255.0f);
  data[3] = static_cast<uint8_t>(value.d / 2.0 * 255.0f);

  return data;
}

template <>
PIDGains NodeEncoder<PIDGains>::Decode(std::array<uint8_t, 4> value) {
  auto g = value[0] / 255.0f * 2.0f;
  auto p = value[1] / 255.0f * 2.0f;
  auto i = value[2] / 255.0f * 2.0f;
  auto d = value[3] / 255.0f * 2.0f;

  return {g, p, i, d};
}

}  // namespace robotics::node
