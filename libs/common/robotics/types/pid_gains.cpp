#pragma once

#include "pid_gains.hpp"
#include "../node/node.hpp"

namespace robotics {
namespace node {

template <>
std::array<uint8_t, 4> NodeEncoder<PIDGains>::Encode(PIDGains value) {
  std::array<uint8_t, 4> data;

  data[0] = value.g / 2.0 * 255.0f;
  data[1] = value.p / 2.0 * 255.0f;
  data[2] = value.i / 2.0 * 255.0f;
  data[3] = value.d / 2.0 * 255.0f;

  return data;
}

template <>
PIDGains NodeEncoder<PIDGains>::Decode(std::array<uint8_t, 4> value) {
  auto g = value[0] / 255.0f * 2.0;
  auto p = value[1] / 255.0f * 2.0;
  auto i = value[2] / 255.0f * 2.0;
  auto d = value[3] / 255.0f * 2.0;

  return PIDGains(g, p, i, d);
}

}  // namespace node
}  // namespace robotics