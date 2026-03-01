#pragma once

#include <driver/rmt_tx.h>
#include <robotics/utils/neopixel_driver.hpp>

namespace robotics::utils {
class RMTDriver : public robotics::utils::NeoPixelDriver {
  static constexpr const char* TAG = "RMTDriver";

  rmt_channel_handle_t rmt_tx_handle = nullptr;
  rmt_encoder_handle_t encoder = nullptr;

  std::vector<uint8_t> buffer_;

  union Errors {
    struct {
      int rmt_channel_create_failed : 1;
      int rmt_encoder_create_failed : 1;
      int transmit_failed : 1;
      int pad : 5;
    };
    std::uint8_t raw = 0;
  } errors;

 public:
  RMTDriver(gpio_num_t gpio_num) {
    rmt_tx_channel_config_t rmt_config{
        .gpio_num = gpio_num,
        .clk_src = rmt_clock_source_t::RMT_CLK_SRC_DEFAULT,
        .resolution_hz = (int)4E6,
        .mem_block_symbols = 96,
        .trans_queue_depth = 100,
        .intr_priority = 0,
        .flags = {.invert_out = 0,
                  .with_dma = 0,
                  .io_loop_back = 0,
                  .io_od_mode = 0}};

    auto ret = rmt_new_tx_channel(&rmt_config, &rmt_tx_handle);
    if (ret != ESP_OK) {
      ESP_LOGE(TAG, "Failed to create RMT TX channel: %d", ret);
      errors.rmt_channel_create_failed = 1;
      return;
    }

    rmt_bytes_encoder_config_t encoder_config = {
        .bit0 = {.duration0 = 1, .level0 = 1, .duration1 = 3, .level1 = 0},
        .bit1 = {.duration0 = 3, .level0 = 1, .duration1 = 1, .level1 = 0},
        .flags = {.msb_first = 1}};
    ret = rmt_new_bytes_encoder(&encoder_config, &encoder);
    if (ret != ESP_OK) {
      ESP_LOGE(TAG, "Failed to create RMT encoder: %d", ret);
      errors.rmt_encoder_create_failed = 1;
    }

    ret = rmt_enable(rmt_tx_handle);
    if (ret != ESP_OK) {
      ESP_LOGE(TAG, "Failed to enable RMT TX channel: %d", ret);
      errors.rmt_channel_create_failed = 1;
    }
  }

  void SetMaxBytes(int bytes) override {
    buffer_.resize(bytes);

    for (int i = 0; i < buffer_.size(); i++) {
      buffer_[i] = 0;
    }
  }

  void Flush() override {
    vTaskDelay(pdMS_TO_TICKS(1));  // reset

    rmt_transmit_config_t tx_config{
        .loop_count = 1, .flags = {.eot_level = 0, .queue_nonblocking = 0}};
    auto ret = rmt_transmit(rmt_tx_handle, encoder, buffer_.data(),
                            buffer_.size(), &tx_config);
    if (ret != ESP_OK) {
      ESP_LOGE(TAG, "Failed to transmit RMT data: %d", ret);
      errors.transmit_failed = 1;
      return;
    }

    ret = rmt_tx_wait_all_done(rmt_tx_handle, 1000);
    if (ret != ESP_OK) {
      ESP_LOGE(TAG, "Failed to wait for RMT transmission to finish: %d", ret);
      errors.transmit_failed = 1;
    }
  }

  void SetByte(int position, uint8_t byte) override {
    buffer_[position] = byte;
  }

  Errors GetErrors() const { return errors; }
};
}  // namespace robotics::utils