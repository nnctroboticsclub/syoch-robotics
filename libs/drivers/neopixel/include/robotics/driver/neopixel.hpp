#pragma once

#include <memory>
#include <vector>

#include "neopixel_driver.hpp"

namespace robotics::utils {
class Color {
 public:
  float r;
  float g;
  float b;

  Color(float r, float g, float b);
  Color(uint32_t rgb);

  Color operator+(Color const& other);
  Color operator-(Color const& other);
  Color operator*(float const& other);
  Color operator*(int const& other);
  Color operator/(int const& other);

  uint32_t ToRGB();

  static Color FromHSV(float h, float s, float v);
};

template <NeoPixelDriver Drv>
class NeoPixel {
  std::shared_ptr<Drv> driver_;
  const size_t kLEDs;

 public:
  NeoPixel(std::shared_ptr<Drv> driver, size_t kLEDs)
      : driver_(driver), kLEDs(kLEDs) {
    driver_->SetMaxBytes(3 * kLEDs);
  }
  void PutPixel(size_t index, uint32_t rgb) {
    uint8_t g = (rgb >> 16) & 0xFF;
    uint8_t r = (rgb >> 8) & 0xFF;
    uint8_t b = rgb & 0xFF;

    driver_->SetByte(index * 3 + 0, r);
    driver_->SetByte(index * 3 + 1, g);
    driver_->SetByte(index * 3 + 2, b);
  }

  void Clear() {
    for (size_t i = 0; i < kLEDs; i++) {
      PutPixel(i, 0);
    }
  }
  void Write() { driver_->Flush(); }
};
}  // namespace robotics::utils