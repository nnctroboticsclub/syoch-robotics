#pragma once

#include <string>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <libstm-ota.hpp>
#include <esp_log.h>

#include <robotics/network/dcan.hpp>

class App {
  static constexpr const char* TAG = "App";

  stm32::ota::InitConfig& GetInitConfig() {
    using stm32::ota::InitConfig;

    static InitConfig init_config = {
        .uarts = {InitConfig::Uart{
            .port = 1,
            .baud_rate = 9600,
            .tx = 21,
            .rx = 20,
            .parity = UART_PARITY_DISABLE,
        }},
        .spi_buses = {InitConfig::SPIBus{
            .port = 1,
            .miso = GPIO_NUM_5,
            .mosi = GPIO_NUM_6,
            .sclk = GPIO_NUM_4,
        }},
        .stm32bls = {InitConfig::STM32BL{
            .id = 1,
            .spi_port_id = 1,
            .cs = GPIO_NUM_7,
        }},
        .stm32s = {InitConfig::STM32{
            .id = 2,
            .reset = GPIO_NUM_0,
            .boot0 = GPIO_NUM_1,
            .bl_id = 1,
        }},
        .serial_proxies = {InitConfig::SerialProxy{.id = 1, .uart_port_id = 1}},
        .network_profiles = {InitConfig::NetworkProfile{
            .id = 0,
            .is_ap = true,
            .is_static = true,
            .ssid = "ESP32",
            .password = "ESP32",
            .hostname = "esp32",
            .ip = 0xc0a80001,
            .subnet = 0xffffff00,
            .gateway = 0xc0a80001,
        }},
        .active_network_profile_id = 0,
        .primary_stm32_id = 2};

    return init_config;
  }
  stm32::ota::OTAServer StartOTAServer() {
    auto& init_config = this->GetInitConfig();
    stm32::ota::OTAServer ota_server(idf::GPIONum(2), init_config);

    ota_server.OnHTTPDStart([this](httpd_handle_t server) {
      //* Register some handlers
    });

    return ota_server;
  }

 public:
  App() {}

  void Init() {}

  void Main() {
    ESP_LOGI(TAG, "Starting OTA Server...");
    stm32::ota::OTAServer ota_server = this->StartOTAServer();

    ESP_LOGI(TAG, "Entering main loop");
    while (1) {
      vTaskDelay(pdMS_TO_TICKS(100));
    }
  }
};