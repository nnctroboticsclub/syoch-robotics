#pragma once

#include <cstdint>

namespace robotics::network {
// FNV-1A
// TODO(syoch): Rename this class to FNV1A
class RD16 {
  uint16_t current = 36683;

 public:
  void Reset() { current = 36683; }

  void operator<<(uint8_t x) {
    current ^= x;
    current *= 37003;
  }

  template <typename T>
  void operator<<(std::vector<T> const& data) {
    for (auto const& x : data) {
      *this << x;
    }
  }

  template <typename T>
  static RD16 FromData(T const& data) {
    RD16 rd16;
    rd16 << data;
    return rd16;
  }

  template <typename T>
  RD16 CopyAndAppend(T const& data) const {
    RD16 rd16 = *this;
    rd16 << data;
    return rd16;
  }

  uint16_t Get() const { return current; }

  void Set(uint16_t x) { current = x; }
};

}  // namespace robotics::network