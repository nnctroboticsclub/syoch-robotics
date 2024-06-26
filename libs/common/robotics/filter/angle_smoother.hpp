#pragma once

#include "../node/node.hpp"

namespace robotics::filter {
template <typename T>
class AngleNormalizer {
  T prev_input = 0;
  T value = 0;

 public:
  Node<T> input;
  Node<T> output;

  Node<T> offset;

  AngleNormalizer() {
    input.SetChangeCallback([this](T _input) {
      T input = _input + offset.GetValue();

      T delta_rot_y_deg = input - prev_input;
      prev_input = input;

      if (delta_rot_y_deg > 180)
        delta_rot_y_deg -= 360;
      else if (delta_rot_y_deg < -180)
        delta_rot_y_deg += 360;

      value += delta_rot_y_deg;

      if (input == 0 && (int)value % 360 == 0) {
        value = 0;
      }

      output.SetValue(value);
    });

    offset.SetChangeCallback(
        [this](T /*offset*/) { input.SetValue(input.GetValue()); });
  }

  void Reset() {
    prev_input = 0;
    value = 0;
  }
};
}  // namespace robotics::filter