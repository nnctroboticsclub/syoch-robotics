#pragma once

#include <cmath>
#include <array>


namespace robotics::inline types {
template <typename T, int N>
class Vector {
  std::array<T, N> data_;

 public:
  Vector() : data_({}) {}

  explicit(false) Vector(std::array<T, N> data) : data_(data) {}

  Vector(T x, T y) : data_({x, y}) {}

  Vector(T x, T y, T z) : data_({x, y, z}) {}

  Vector(T x, T y, T z, T w) : data_({x, y, z, w}) {}

  Vector operator+(Vector const& rhs) const {
    std::array<T, N> data;
    for (int i = 0; i < N; i++) {
      data[i] = data_[i] + rhs.data_[i];
    }
    return Vector(data);
  }

  Vector operator-(Vector const& rhs) const {
    std::array<T, N> data;
    for (int i = 0; i < N; i++) {
      data[i] = data_[i] - rhs.data_[i];
    }
    return Vector(data);
  }

  Vector operator*(T const& rhs) const {
    std::array<T, N> data;
    for (int i = 0; i < N; i++) {
      data[i] = data_[i] * rhs;
    }
    return Vector(data);
  }

  Vector operator/(T const& rhs) const {
    std::array<T, N> data;
    for (int i = 0; i < N; i++) {
      data[i] = data_[i] / rhs;
    }
    return Vector(data);
  }

  bool operator==(Vector const& rhs) const {
    for (int i = 0; i < N; i++) {
      if (data_[i] != rhs.data_[i]) {
        return false;
      }
    }
    return true;
  }

  bool operator!=(Vector const& rhs) const { return !(*this == rhs); }

  T& operator[](int i) { return data_[i]; }

  T const& operator[](int i) const { return data_[i]; }

  T Magnitude() const {
    T sum = 0;
    for (int i = 0; i < N; i++) {
      sum += data_[i] * data_[i];
    }
    return sqrt(sum);
  }

  Vector Normalized() const { return *this / Magnitude(); }

  T Dot(Vector const& rhs) const {
    T sum = 0;
    for (int i = 0; i < N; i++) {
      sum += data_[i] * rhs.data_[i];
    }
    return sum;
  }

  Vector Cross(Vector const rhs) const {
    static_assert(N == 3, "Cross product is only defined for 3D vectors");
    return Vector(data_[1] * rhs.data_[2] - data_[2] * rhs.data_[1],
                  data_[2] * rhs.data_[0] - data_[0] * rhs.data_[2],
                  data_[0] * rhs.data_[1] - data_[1] * rhs.data_[0]);
  }
};
} // namespace robotics::inline types

