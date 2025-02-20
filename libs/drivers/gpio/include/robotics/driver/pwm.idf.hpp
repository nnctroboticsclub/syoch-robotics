#pragma once

#include "pwm.hpp"

#include <driver/ledc.h>

/*  timing diagram of PWM signal (where lpoint = overflow)

     overflow |----------+----------+
         /\   |         /|         /|
         ||   |        / |        / |
    duty ||   |       /  |       /  |
         ||   |      /   |      /   |
         \/   |     /    |     /    |
       hpoint |----+     |----+     |
            0 |   /|     |   /|     |
              |  / |     |  / |     |
              | /  |     | /  |     |
              |/   |     |/   |     |

              |    +-----|    +-----|
    => signal |    |     |    |     |
              |----+     |----+     |

              |<-------->|<-------->|: period [cnt]
              |    |<--->|    |<--->|: hduty  [cnt]
              |<-->|     |<-->|     |: lduty  [cnt]

    lduty[%] = 100% * hpoint / overflow

         lduty[%] + hduty[%] = 100%
                      hpoint = overflow * (1 - hduty[%])

    duty[cnt] = lpoint - hpoint
              = overflow * hduty[%]
*/

#define LEDC_MODE LEDC_HIGH_SPEED_MODE

namespace robotics::driver {
class PWM : public PWMBase {
  int period_us_ = 1000;
  int pulsewidth_us_ = 0;

  ledc_channel_t ledc_channel_;
  ledc_timer_t ledc_timer_;

  void Update() {
    float hduty = (float)pulsewidth_us_ / period_us_;
    const int overflow = 1024;
    const int hpoint = overflow * (1 - hduty);
    const int duty = overflow * hduty;

    ledc_set_duty_with_hpoint(LEDC_MODE, ledc_channel_, duty, hpoint);
    ledc_update_duty(LEDC_MODE, ledc_channel_);
  }

 public:
  PWM(gpio_num_t pin, ledc_channel_t ledc_channel, ledc_timer_t ledc_timer)
      : ledc_channel_(ledc_channel), ledc_timer_(ledc_timer) {
    ledc_timer_config_t tconfig = {
        .speed_mode = LEDC_MODE,
        .duty_resolution = LEDC_TIMER_10_BIT,
        .timer_num = ledc_timer,
        .freq_hz = 1000,
        .clk_cfg = LEDC_AUTO_CLK,
    };
    ledc_timer_config(&tconfig);
    ledc_set_duty_with_hpoint(LEDC_MODE, ledc_channel, 1024, 0);

    ledc_channel_config_t cconfig = {.gpio_num = pin,
                                     .speed_mode = LEDC_MODE,
                                     .channel = ledc_channel,
                                     .intr_type = LEDC_INTR_DISABLE,
                                     .timer_sel = ledc_timer,
                                     .duty = 1024,
                                     .hpoint = 0,
                                     .flags = {.output_invert = 0}};
    ledc_channel_config(&cconfig);
  }

  void pulsewidth_us(int pw) override {
    pulsewidth_us_ = pw;

    Update();
  }

  void period_us(int period) override {
    pulsewidth_us_ = period * pulsewidth_us_ / period_us_;
    period_us_ = period;

    ledc_set_freq(LEDC_MODE, ledc_timer_, 1000000 / period_us_);

    Update();
  }
};
}  // namespace robotics::driver