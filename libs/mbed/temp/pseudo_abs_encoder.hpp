#pragma once

#include <mbed.h>

namespace robotics::mbed {
class PseudoAbsEncoder {
 private:
  int pulses_per_rotation = 200;

  int current_pulses = 0;
  bool is_abs_ready = false;

  ::mbed::InterruptIn A_;
  ::mbed::InterruptIn B_;
  ::mbed::InterruptIn index_;

 public:
  struct Config {
    PinName a, b, index;
  };

  PseudoAbsEncoder(PinName A, PinName B, PinName index)
      : A_(A), B_(B), index_(index) {
    A_.rise([this]() {
      auto b = B_.read();
      if (b) {
        current_pulses += 1;
      } else {
        current_pulses -= 1;
      }
    });
    A_.fall([this]() {
      auto b = B_.read();
      if (b) {
        current_pulses -= 1;
      } else {
        current_pulses += 1;
      }
    });
    index_.rise([this]() {
      if (is_abs_ready) return;
      current_pulses = 0;
      is_abs_ready = true;
    });
  }

  PseudoAbsEncoder(Config const& config)
      : PseudoAbsEncoder(config.a, config.b, config.index) {}

  double GetAngle() { return 360 * (current_pulses / 2) / pulses_per_rotation; }
  int GetPulses() { return current_pulses; }

  bool IsAbsReady() { return is_abs_ready; }
};
}  // namespace robotics::mbed