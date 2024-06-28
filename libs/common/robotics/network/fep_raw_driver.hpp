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

enum class ResultType { kOk, kError };

struct DriverResult {
  ResultType type;
  int value;

  bool Failed() const;
};

struct FEPPacket {
  uint8_t from;
  uint8_t length;
  uint8_t data[64];
};

struct FEPRawLine {
  char line[32];
};

enum class State {
  kIdle,
  kProcessing,
};

class RxProcessor;  // Defined in the .cpp file

class FEP_RawDriver : public Stream<uint8_t, uint8_t, TxState> {
  Stream<uint8_t>& upper_stream;

  robotics::utils::NoMutexLIFO<DriverResult, 4> result_queue_;
  robotics::utils::NoMutexLIFO<FEPPacket, 4> rx_queue_;
  robotics::utils::NoMutexLIFO<FEPRawLine, 4> line_queue_;

  RxProcessor* rx_processor_;

  State state_ = State::kIdle;

  system::Timer timer_;

  void Send(std::string const& data);

  [[nodiscard]]
  types::Result<int, DriverError> WaitForState(
      State state, std::chrono::milliseconds timeout = 1000ms);

  [[nodiscard]]
  types::Result<FEPRawLine, DriverError> ReadLine(
      std::chrono::milliseconds timeout = 1000ms);

  [[nodiscard]]
  types::Result<DriverResult, DriverError> ReadResult(
      std::chrono::milliseconds timeout = 3000ms);

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