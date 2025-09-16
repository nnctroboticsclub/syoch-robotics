#pragma once

#include <mbed.h>

#include <cstdint>

#include <logger/logger.hpp>
#include <robotics/network/iuart.hpp>
#include <robotics/network/stream.hpp>
#include <robotics/thread/thread.hpp>
#include "robotics/utils/no_mutex_lifo.hpp"

namespace robotics::network {
class UARTStream : public IUART {
  static robotics::logger::Logger logger;

  utils::NoMutexLIFO<uint8_t, 32> buffer;
  mbed::UnbufferedSerial* upper_stream = nullptr;
  robotics::system::Thread thread_dispatch;
  PinName tx, rx;

  bool is_running = false;
  bool stop_token = false;

  void Init(int baudrate);

  void Deinit();

 public:
  UARTStream(PinName tx, PinName rx, int baud);

  ~UARTStream();

  void Send(uint8_t* data, uint32_t len) override;

  void Rebaud(int baud) override;
};

}  // namespace robotics::network