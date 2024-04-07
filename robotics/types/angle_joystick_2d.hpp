#pragma once
namespace robotics {
inline namespace types {

class AngleStick2D {
 public:
  float magnitude;
  float angle;  // clockwise

  AngleStick2D();
  AngleStick2D(float magnitude, float angle);

  bool operator==(AngleStick2D const& other) const;
  bool operator!=(AngleStick2D const& other) const;
};

}  // namespace types
}  // namespace robotics