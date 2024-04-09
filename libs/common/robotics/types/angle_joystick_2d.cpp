#include "angle_joystick_2d.hpp"

namespace robotics {
inline namespace types {

AngleStick2D::AngleStick2D() : magnitude(0), angle(0) {}
AngleStick2D::AngleStick2D(float magnitude, float angle)
    : magnitude(magnitude), angle(angle) {}

bool AngleStick2D::operator==(AngleStick2D const& other) const {
  return magnitude == other.magnitude && angle == other.angle;
}
bool AngleStick2D::operator!=(AngleStick2D const& other) const {
  return !(*this == other);
}
};  // namespace types

}  // namespace robotics