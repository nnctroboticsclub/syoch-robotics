#pragma once

#include "vector.hpp"

#include <robotics/node/node.hpp>

namespace robotics {
inline namespace types {

struct PIDGains {
  float g;
  float p;
  float i;
  float d;

  PIDGains() : g(0), p(0), i(0), d(0) {}
  PIDGains(float g, float p, float i, float d) : g(g), p(p), i(i), d(d) {}

  void Normalize() {
    float x = p + i + d;

    if (x == 0) return;

    p /= x;
    i /= x;
    d /= x;
  }

  bool operator==(PIDGains const& other) const {
    return g == other.g && p == other.p && i == other.i && d == other.d;
  }
  bool operator!=(PIDGains const& other) const { return !(*this == other); }
};

}  // namespace types

namespace node {
template <>
struct NodeEncoderExistsType<PIDGains> {
  using value = std::true_type;
};
}  // namespace node
}  // namespace robotics