#pragma once

#include <cstdint>

#include <chrono>
#include <sstream>
#include <string>
#include <iomanip>

#include "stream.hpp"

#include "../logger/logger.hpp"
#include "../utils/no_mutex_lifo.hpp"
#include "../types/result.hpp"
#include "../platform/timer.hpp"

#include "fep_tx_state.hpp"

namespace robotics::network {

namespace fep {
using namespace std::chrono_literals;

class DriverError : public std::string {
 public:
  DriverError();
  DriverError(std::string const& message);
};

struct DriverResult {
  enum class Type { kOk, kError };

  Type type;
  int value;

  bool Failed() const;
};

class FEP_RawDriver : public Stream<uint8_t, uint8_t, TxState> {
  Stream<uint8_t>& upper_stream;

  robotics::utils::NoMutexLIFO<char, 64> rx_queue_;
  robotics::utils::NoMutexLIFO<DriverResult, 4> result_queue_;

  enum class Flags {
    kRxOverflow = 1 << 0,
  };

  uint32_t flags_ = 0;

  enum class State {
    kIdle,
    kProcessing,
    kRxData,
    kRxResult,
  };

  State state_ = State::kIdle;

  system::Timer timer_;

  uint8_t on_binary_data_buffer[128];

  void Send(std::string const& data);

  void ISR_ParseBinary();
  void ISR_ParseResult();

  void ISR_OnUARTData(uint8_t* buffer, uint32_t length);

  [[nodiscard]]
  types::Result<int, DriverError> WaitForState(
      State state, std::chrono::milliseconds timeout = 1000ms);

  [[nodiscard]]
  types::Result<std::string, DriverError> ReadLine(
      std::chrono::milliseconds timeout = 1000ms);

  [[nodiscard]]
  types::Result<DriverResult, DriverError> ReadResult(
      std::chrono::milliseconds timeout = 1000ms);

 public:
  FEP_RawDriver(Stream<uint8_t>& upper_stream);

  [[nodiscard]]
  types::Result<uint8_t, DriverError> GetRegister(
      uint8_t address, std::chrono::milliseconds timeout = 1000ms);

  [[nodiscard]]
  types::Result<DriverResult, DriverError> SetRegister(uint8_t address,
                                                       uint8_t value);

  [[nodiscard]]
  types::Result<DriverResult, DriverError> Reset();

  [[nodiscard]]
  types::Result<DriverResult, DriverError> InitAllRegister();

  [[nodiscard]]
  TxState Send(uint8_t address, uint8_t* data, uint32_t length) override;

};
}  // namespace fep

using fep::FEP_RawDriver;
}  // namespace robotics::network