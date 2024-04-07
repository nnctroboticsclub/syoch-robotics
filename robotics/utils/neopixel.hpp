#pragma once

#include <vector>

#include <mbed.h>

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
};

class NeoPixel {
  static constexpr int kResetSize = 48 * 2;
  static std::vector<uint8_t> reset;
  mbed::SPI spi;
  const size_t kLEDs;
  std::vector<uint8_t> data;

  void WriteByte(size_t index, uint8_t byte);

 public:
  NeoPixel(PinName pin, size_t kLEDs);

  void PutPixel(size_t index, uint32_t rgb);

  void Clear();
  void Write();

  void Debug();
};
}  // namespace robotics::utils