#pragma once

namespace robotics {
inline namespace types {
enum class AngleType {
  kRadians,
  kDegrees,
};

template <typename T>
class BaseAngle {
 public:
  T value;

  BaseAngle() : value(0) {}
  BaseAngle(T value) : value(value) {}

  BaseAngle<T> operator+(BaseAngle<T> const& rhs) const {
    return BaseAngle<T>(value + rhs.value);
  }

  BaseAngle<T> operator-(BaseAngle<T> const& rhs) const {
    return BaseAngle<T>(value - rhs.value);
  }

  BaseAngle<T> operator*(T const& rhs) const {
    return BaseAngle<T>(value * rhs);
  }

  BaseAngle<T> operator/(T const& rhs) const {
    return BaseAngle<T>(value / rhs);
  }

  BaseAngle<T>& operator+=(BaseAngle<T> const& rhs) {
    value += rhs.value;
    return *this;
  }

  BaseAngle<T>& operator-=(BaseAngle<T> const& rhs) {
    value -= rhs.value;
    return *this;
  }

  BaseAngle<T>& operator*=(T const& rhs) {
    value *= rhs;
    return *this;
  }

  BaseAngle<T>& operator/=(T const& rhs) {
    value /= rhs;
    return *this;
  }
};

template <typename T, AngleType type = AngleType::kDegrees>
class Angle {};

template <typename T>
class Angle<T, AngleType::kDegrees> : public BaseAngle<T> {
 public:
  using BaseAngle<T>::BaseAngle;

  Angle<T, AngleType::kRadians> ToRadians() const {
    return Angle<T, AngleType::kRadians>(this->value * M_PI / 180);
  }
};

template <typename T>
class Angle<T, AngleType::kRadians> : public BaseAngle<T> {
 public:
  using BaseAngle<T>::BaseAngle;

  Angle<T, AngleType::kDegrees> ToDegrees() const {
    return Angle<T, AngleType::kDegrees>(this->value * 180 / M_PI);
  }
};

}  // namespace types

}  // namespace robotics