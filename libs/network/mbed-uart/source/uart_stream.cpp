#include <robotics/network/uart_stream.hpp>

namespace robotics::network {
void UARTStream::Init(int baudrate) {
  if (upper_stream) return;

  upper_stream = new mbed::UnbufferedSerial(tx, rx, baudrate);
  upper_stream->enable_input(true);
  upper_stream->enable_output(true);
}

void UARTStream::Deinit() {
  if (!upper_stream) return;
  upper_stream->enable_input(false);
  upper_stream->enable_output(false);
  upper_stream->close();
  delete upper_stream;
  upper_stream = nullptr;
}

UARTStream::UARTStream(PinName tx, PinName rx, int baud) : tx(tx), rx(rx) {
  Init(baud);

  thread.SetThreadName("UARTStream");
  thread.SetStackSize(8192);
  thread.Start([this]() {
    this->is_running = true;
    logger.Info("UARTStream is starting");
    while (!upper_stream) {
      robotics::system::SleepFor(10ms);
    }

    logger.Info("UARTStream is running");

    while (!stop_token) {
      while (upper_stream->readable()) {
        uint8_t data[128];
        uint32_t length = upper_stream->read(&data, 1024);
        DispatchOnReceive(data, length);
      }
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
  if (!upper_stream) return;
  upper_stream->write(data, len);
}

void UARTStream::Rebaud(int baud) {
  Deinit();
  Init(baud);
}

robotics::logger::Logger UARTStream::logger =
    robotics::logger::Logger("UARTStream", "uart.mbed.nw");
}  // namespace robotics::network