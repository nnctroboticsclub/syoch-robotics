#include <robotics/driver/neopixel.hpp>

#include <cmath>
#include <cstdio>

namespace robotics::utils {
Color::Color(float r, float g, float b) : r(r), g(g), b(b) {}

Color::Color(uint32_t rgb)
    : r((rgb >> 16) & 0xFF), g((rgb >> 8) & 0xFF), b(rgb & 0xFF) {}

Color Color::operator+(Color const& other) {
  return {r + other.r, g + other.g, b + other.b};
}

Color Color::operator-(Color const& other) {
  return {r - other.r, g - other.g, b - other.b};
}

Color Color::operator*(float const& other) {
  return {r * other, g * other, b * other};
}

Color Color::operator*(int const& other) {
  return {r * other, g * other, b * other};
}

Color Color::operator/(int const& other) {
  return {r / other, g / other, b / other};
}

uint32_t Color::ToRGB() {
  auto r = (uint8_t)(this->r > 255 ? 255 : this->r);
  auto g = (uint8_t)(this->g > 255 ? 255 : this->g);
  auto b = (uint8_t)(this->b > 255 ? 255 : this->b);

  return (r << 16) | (g << 8) | b;
}

Color Color::FromHSV(float h, float s, float v) {
  float c = v * s;
  float x = c * (1 - std::abs(fmod(h / 60, 2) - 1));
  float m = v - c;

  float r, g, b;
  if (h < 60) {
    r = c;
    g = x;
    b = 0;
  } else if (h < 120) {
    r = x;
    g = c;
    b = 0;
  } else if (h < 180) {
    r = 0;
    g = c;
    b = x;
  } else if (h < 240) {
    r = 0;
    g = x;
    b = c;
  } else if (h < 300) {
    r = x;
    g = 0;
    b = c;
  } else {
    r = c;
    g = 0;
    b = x;
  }

  return Color((r + m) * 255, (g + m) * 255, (b + m) * 255);
}

}  // namespace robotics::utils