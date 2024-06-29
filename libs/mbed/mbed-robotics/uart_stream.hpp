#pragma once

#include <mbed.h>

#include <string>

#include <robotics/network/stream.hpp>

namespace robotics::network {
class UARTStream : public Stream<uint8_t> {
  logger::Logger logger{"uart.nw", "\x1b[1;35m   UART  \x1b[m"};
  mbed::UnbufferedSerial* upper_stream = nullptr;

  PinName tx, rx;

  void Init(int baudrate) {
    if (upper_stream) return;

    upper_stream = new mbed::UnbufferedSerial(tx, rx, baudrate);
    upper_stream->enable_input(true);
    upper_stream->enable_output(true);
    upper_stream->attach([this]() {
      while (upper_stream->readable()) {
        uint8_t data[128];
        uint32_t length = upper_stream->read(&data, 1024);
        DispatchOnReceive(data, length);
      }
    });
  }

  void Deinit() {
    if (!upper_stream) return;
    upper_stream->enable_input(false);
    upper_stream->enable_output(false);
    upper_stream->close();
    delete upper_stream;
    upper_stream = nullptr;
  }

 public:
  UARTStream(PinName tx, PinName rx, int baud) : tx(tx), rx(rx) { Init(baud); }

  void Send(uint8_t* data, uint32_t len) override {
    if (!upper_stream) return;
    upper_stream->write(data, len);
  }

  void Rebaud(int baud) {
    Deinit();
    Init(baud);
  }
};
}  // namespace robotics::network