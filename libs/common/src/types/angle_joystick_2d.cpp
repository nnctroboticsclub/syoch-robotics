#include <robotics/types/angle_joystick_2d.hpp>

namespace robotics::inline types {

AngleStick2D::AngleStick2D() : magnitude(0), angle(0) {}
AngleStick2D::AngleStick2D(float magnitude, float angle)
    : magnitude(magnitude), angle(angle) {}

bool AngleStick2D::operator==(AngleStick2D const& other) const = default;
}  // namespace robotics::inline types
