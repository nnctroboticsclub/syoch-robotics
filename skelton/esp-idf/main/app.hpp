#pragma once

#include <string>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <esp_log.h>

#include <robotics/network/dcan.hpp>

class App {
  static constexpr const char* TAG = "App";

 public:
  App() {}

  void Init() {}

  void Main() {
    ESP_LOGI(TAG, "Entering main loop");
    while (1) {
      vTaskDelay(pdMS_TO_TICKS(100));
    }
  }
};