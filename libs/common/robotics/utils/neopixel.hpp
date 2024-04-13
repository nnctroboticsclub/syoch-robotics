#pragma once

#include <vector>
#include <memory>

#include "neopixel_driver.hpp"
#include "../platform/spi.hpp"

namespace robotics::utils {
class Color {
 public:
  float r;
  float g;
  float b;

  Color(float r, float g, float b);
  Color(uint32_t rgb);

  Color operator+(Color const &other);
  Color operator-(Color const &other);
  Color operator*(float const &other);
  Color operator*(int const &other);
  Color operator/(int const &other);

  uint32_t ToRGB();

  static Color FromHSV(float h, float s, float v);
};

class NeoPixel {
  std::shared_ptr<NeoPixelDriver> driver_;
  const size_t kLEDs;

 public:
  NeoPixel(std::shared_ptr<NeoPixelDriver> driver, size_t kLEDs);

  void PutPixel(size_t index, uint32_t rgb);

  void Clear();
  void Write();
};
}  // namespace robotics::utils