#pragma once

#include <mbed.h>

#include <string>

#include <robotics/network/stream.hpp>

namespace robotics::network {
class UARTStream : public Stream<uint8_t> {
  mbed::UnbufferedSerial upper_stream;

 public:
  void Send(uint8_t* data, uint32_t len) override {
    upper_stream.write(data, len);
  }

  UARTStream(PinName tx, PinName rx, int baud) : upper_stream(tx, rx, baud) {
    upper_stream.attach([this]() {
      while (upper_stream.readable()) {
        uint8_t data[128];
        uint32_t length = upper_stream.read(&data, 1024);
        DispatchOnReceive(data, length);
      }
    });
  }
};
}  // namespace robotics::network