#pragma once

#include <mbed.h>

#include <cstdint>

#include <Nano/no_mutex_lifo.hpp>
#include <logger/logger.hpp>
#include <robotics/network/iuart.hpp>
#include <robotics/network/stream.hpp>
#include "NanoHW/thread.hpp"

namespace robotics::network {
class UARTStream : public IUART {
  static robotics::logger::Logger logger;

  Nano::collection::NoMutexLIFO<uint8_t, 32> buffer;
  mbed::UnbufferedSerial* upper_stream = nullptr;
  nano_hw::thread::DynThread thread_dispatch{ThreadPriorityNormal, 8192,
                                             nullptr, "UARTStream-Dispatch"};
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