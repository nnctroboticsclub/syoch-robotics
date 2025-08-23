#include <algorithm>
#include <robotics/network/uart_stream.hpp>
#include "robotics/thread/thread.hpp"

namespace robotics::network {
void UARTStream::Init(int baudrate) {
  if (upper_stream)
    return;

  upper_stream = new mbed::UnbufferedSerial(tx, rx, baudrate);
  upper_stream->enable_input(true);
  upper_stream->enable_output(true);
}

void UARTStream::Deinit() {
  if (!upper_stream)
    return;
  upper_stream->enable_input(false);
  upper_stream->enable_output(false);
  upper_stream->close();
  delete upper_stream;
  upper_stream = nullptr;
}

UARTStream::UARTStream(PinName tx, PinName rx, int baud) : tx(tx), rx(rx) {
  Init(baud);

  thread_recv.SetThreadName("UARTStream");
  thread_recv.SetStackSize(8192);
  thread_recv.Start([this]() {
    this->is_running = true;
    logger.Info("UARTStream is starting");
    while (!upper_stream) {
      robotics::system::SleepFor(10ms);
    }

    logger.Info("UARTStream is running");

    while (!stop_token) {
      if (upper_stream->readable()) {
        static uint8_t buf[128] = {};
        const auto len = upper_stream->read(buf, 128);

        buffer.PushN(buf, len);
      }
    }

    logger.Info("UARTStream is stopping");

    this->is_running = false;
  });

  thread_dispatch.SetThreadName("UARTStream-Dispatch");
  thread_dispatch.SetStackSize(8192);
  thread_dispatch.Start([this]() {
    this->is_running = true;
    logger.Info("UARTStream is starting");
    while (!upper_stream) {
      robotics::system::SleepFor(10ms);
    }

    logger.Info("UARTStream is running");

    while (!stop_token) {
      // logger.Info("upper_stream.readable() -> %d, buffer.Size() -> %d",
      //             upper_stream->readable(), buffer.Size());
      if (not buffer.Empty()) {
        static uint8_t buf[128] = {};
        auto len = buffer.Size() < 128 ? buffer.Size() : 128;
        buffer.PopNTo(len, buf);
        DispatchOnReceive(buf, len);
      }

      robotics::system::SleepFor(100ms);
    }

    logger.Info("UARTStream is stopping");

    this->is_running = false;
  });

  while (!is_running) {
    robotics::system::SleepFor(10ms);
  }
}

UARTStream::~UARTStream() {
  stop_token = true;
  while (is_running) {
    robotics::system::SleepFor(10ms);
  }
  Deinit();
}

void UARTStream::Send(uint8_t* data, uint32_t len) {
  if (!upper_stream)
    return;
  upper_stream->write(data, len);
}

void UARTStream::Rebaud(int baud) {
  Deinit();
  Init(baud);
}

robotics::logger::Logger UARTStream::logger =
    robotics::logger::Logger("UARTStream", "uart.mbed.nw");
}  // namespace robotics::network