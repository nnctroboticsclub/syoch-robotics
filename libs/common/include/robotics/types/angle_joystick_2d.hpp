#pragma once

#include <robotics/node/node.hpp>

namespace robotics {
inline namespace types {

class AngleStick2D {
 public:
  float magnitude;
  float angle;  // clockwise

  AngleStick2D();
  AngleStick2D(float magnitude, float angle);

  bool operator==(AngleStick2D const& other) const;
};

}  // namespace types

namespace node {
template <>
struct NodeEncoderExistsType<AngleStick2D> {
  using value = std::true_type;
};
}  // namespace node
}  // namespace robotics