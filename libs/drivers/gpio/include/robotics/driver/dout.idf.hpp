#pragma once

#include "dout.hpp"

#include <driver/gpio.h>

namespace robotics::driver {
class Dout : public IDout {
 private:
  gpio_num_t pin_;

 public:
  Dout(gpio_num_t pin) : pin_(pin) {
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = 1ULL << pin_;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&io_conf);
  }

  bool Write(bool value) override {
    gpio_set_level(pin_, value);
    return true;
  }
};
}  // namespace robotics::driver