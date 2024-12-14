#pragma once

#include "vector.hpp"
#include <robotics/node/node.hpp>

namespace robotics {
inline namespace types {

using JoyStick2D = Vector<float, 2>;

}  // namespace types
namespace node {
template <>
struct NodeEncoderExistsType<JoyStick2D> {
  using value = std::true_type;
};
}  // namespace node
}  // namespace robotics