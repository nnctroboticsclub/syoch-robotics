#pragma once

#include <mbed.h>

#include <string>

#include <robotics/network/stream.hpp>
#include <robotics/network/iuart.hpp>
#include <robotics/thread/thread.hpp>
#include <logger/logger.hpp>

namespace robotics::network {
class UARTStream : public IUART {
  static robotics::logger::Logger logger;

  mbed::UnbufferedSerial* upper_stream = nullptr;
  robotics::system::Thread thread;
  PinName tx, rx;

  bool is_running = false;
  bool stop_token = false;

  void Init(int baudrate);

  void Deinit() ;

 public:
  UARTStream(PinName tx, PinName rx, int baud) ;

  ~UARTStream() ;

  void Send(uint8_t* data, uint32_t len) override ;

  void Rebaud(int baud) ;
};

}  // namespace robotics::network