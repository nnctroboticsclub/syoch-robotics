#pragma once

#include <cstdint>

#include <chrono>
#include <sstream>
#include <string>
#include <iomanip>

#include <robotics/network/stream.hpp>

#include <logger/logger.hpp>
#include <robotics/utils/no_mutex_lifo.hpp>
#include <robotics/types/result.hpp>
#include <robotics/timer/timer.hpp>
#include <robotics/thread/thread.hpp>

#include "fep_tx_state.hpp"
#include "fep_packet.hpp"
#include "result.hpp"

namespace robotics::network {

namespace fep {
using namespace std::chrono_literals;

class DriverError : public std::string {
 public:
  DriverError();
  DriverError(std::string const& message);
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
  robotics::utils::NoMutexLIFO<FEPPacket, 16> rx_queue_;
  robotics::utils::NoMutexLIFO<FEPRawLine, 4> line_queue_;

  bool rx_enabled = true;

  RxProcessor* rx_processor_;

  State state_ = State::kIdle;

  system::Timer timer_;
  system::Thread fep_thread_;

  void Send(std::string const& data);

  [[nodiscard]]
  types::Result<int, DriverError> WaitForState(
      State state, std::chrono::milliseconds timeout = 2000ms);

  [[nodiscard]]
  types::Result<FEPRawLine, DriverError> ReadLine(
      std::chrono::milliseconds timeout = 2000ms);

  [[nodiscard]]
  types::Result<DriverResult, DriverError> ReadResult(
      std::chrono::milliseconds timeout = 5000ms);

 public:
  FEP_RawDriver(Stream<uint8_t>& upper_stream);

  [[nodiscard]]
  types::Result<uint8_t, DriverError> GetRegister(
      uint8_t address, std::chrono::milliseconds timeout = 2000ms);

  [[nodiscard]]
  types::Result<DriverResult, DriverError> SetRegister(uint8_t address,
                                                       uint8_t value);

  [[nodiscard]]
  types::Result<DriverResult, DriverError> Reset();

  void ResetNoResult();

  [[nodiscard]]
  types::Result<DriverResult, DriverError> InitAllRegister();

  [[nodiscard]]
  types::Result<FEPRawLine, DriverError> Version(
      std::chrono::milliseconds timeout = 2000ms);

  [[nodiscard]]
  TxState Send(uint8_t address, uint8_t* data, uint32_t length) override;

  void FlushQueue();

  void SetDispatchRX(bool enabled);
};
}  // namespace fep

using fep::FEP_RawDriver;
}  // namespace robotics::network