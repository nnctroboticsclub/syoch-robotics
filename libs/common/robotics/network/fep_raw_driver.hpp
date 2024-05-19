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
  DriverError() : std::string() {}

  DriverError(std::string const& message) : std::string(message) {}
};

class FEP_RawDriver : public Stream<uint8_t, uint8_t, TxState> {
  Stream<uint8_t>& upper_stream;

  robotics::utils::NoMutexLIFO<char, 64> rx_queue_;

  enum class Flags {
    kRxOverflow = 1 << 0,
  };

  uint32_t flags_ = 0;

  enum class State {
    kIdle,
    kProcessing,
    kRxData,
  };

  State state_ = State::kIdle;

  system::Timer timer_;

  uint8_t on_binary_data_buffer[128];

  void Send(std::string const& data) {
    upper_stream.Send((uint8_t*)data.data(), (uint32_t)data.size());
  }

  void ISR_ParseBinary() {
    if (rx_queue_.Size() < 9) {
      return;
    }

    uint8_t address;
    uint32_t length;

    address = (rx_queue_[3] - '0') * 100 + (rx_queue_[4] - '0') * 10 +
              (rx_queue_[5] - '0');

    length = (rx_queue_[6] - '0') * 100 + (rx_queue_[7] - '0') * 10 +
             (rx_queue_[8] - '0');

    // Checks if there is enough data in the queue
    if (rx_queue_.Size() < 9 + length + 2) {
      return;
    }

    // Skips "RBN" + AAA + LLL, where AAA is the address and LLL is the length
    for (size_t i = 0; i < 9; i++) {
      rx_queue_.Pop();
    }

    for (int i = 0; i < length; i++) {
      on_binary_data_buffer[i] = rx_queue_.Pop();
    }

    // Skips the "\r\n" at the end of the binary data
    for (size_t i = 0; i < 2; i++) {
      rx_queue_.Pop();
    }

    DispatchOnReceive(address, on_binary_data_buffer, length);

    state_ = State::kIdle;
  }

  void ISR_OnUARTData(uint8_t* buffer, uint32_t length) {
    // logger::LogCh("FEP-RX", buffer[0]);

    for (int i = 0; i < length; i++) {
      if (!rx_queue_.Push(buffer[i])) {
        flags_ |= (uint32_t)Flags::kRxOverflow;
      }
    }

    if (rx_queue_[0] == 'R' && rx_queue_[1] == 'B' && rx_queue_[2] == 'N') {
      state_ = State::kRxData;
      ISR_ParseBinary();
    }
  }

  [[nodiscard]]
  types::Result<int, DriverError> WaitForState(
      State state, std::chrono::milliseconds timeout = 1000ms) {
    auto start = timer_.ElapsedTime();

    while (1) {
      if (state_ == state) {
        return 0;
      }

      if (timer_.ElapsedTime() > start + timeout) {
        return DriverError("State Timeout");
      }
    }
  }

  [[nodiscard]]
  types::Result<std::string, DriverError> ReadLine(
      std::chrono::milliseconds timeout = 1000ms) {
    std::string line;

    auto start = timer_.ElapsedTime();

    while (1) {
      if (rx_queue_.Empty()) {
        if (timer_.ElapsedTime() > start + timeout) {
          return DriverError("Timeout");
        }
        continue;
      }

      auto c = rx_queue_.Pop();
      if (c == '\r') {
        continue;
      }
      if (c == '\n') {
        break;
      }

      line.push_back(c);
    }

    return line;
  }

  [[nodiscard]]
  types::Result<int, DriverError> ReadResult(
      std::chrono::milliseconds timeout = 1000ms) {
    auto line_res = ReadLine(timeout);
    if (!line_res.IsOk()) {
      return line_res.UnwrapError();
    }

    auto line = line_res.Unwrap();
    int value;
    char c;
    std::stringstream ss;
    ss << line;
    ss >> c >> value;

    if (c == 'N') {
      if (value == 0) return -10;
      return -value;
    } else if (c == 'P') {
      return value;
    } else {
      ss.str("");
      ss << "Invalid response: " << line;
      return DriverError(ss.str());
    }
  }

 public:
  FEP_RawDriver(Stream<uint8_t>& upper_stream) : upper_stream(upper_stream) {
    timer_.Reset();
    timer_.Start();
    upper_stream.OnReceive([this](uint8_t* buffer, uint32_t length) {
      ISR_OnUARTData((uint8_t*)buffer, length);
    });
  }

  [[nodiscard]]
  types::Result<uint8_t, DriverError> GetRegister(
      uint8_t address, std::chrono::milliseconds timeout = 1000ms) {
    auto wait_for_state = WaitForState(State::kIdle, timeout);
    if (!wait_for_state.IsOk()) {
      return wait_for_state.UnwrapError();
    }

    std::stringstream ss;
    ss << "@REG" << std::setw(2) << std::setfill('0') << (int)address << "\r\n";
    Send(ss.str());
    ss.str("");

    state_ = State::kProcessing;

    auto line_res = ReadLine(timeout);
    if (!line_res.IsOk()) {
      return line_res.UnwrapError();
    }

    auto line = line_res.Unwrap();
    int value;
    ss << std::hex << line;
    ss >> value;

    state_ = State::kIdle;
    return value;
  }

  [[nodiscard]]
  types::Result<int, DriverError> SetRegister(uint8_t address, uint8_t value) {
    auto wait_for_state = WaitForState(State::kIdle);
    if (!wait_for_state.IsOk()) {
      return wait_for_state.UnwrapError();
    }
    state_ = State::kProcessing;

    std::stringstream ss;
    ss << "@REG" << std::setw(2) << std::setfill('0') << (int)address << ":"
       << std::setw(3) << std::setfill('0') << (int)value << "\r\n";
    Send(ss.str());

    state_ = State::kIdle;
    return ReadResult();
  }

  [[nodiscard]]
  types::Result<int, DriverError> Reset() {
    auto wait_for_state = WaitForState(State::kIdle);
    if (!wait_for_state.IsOk()) {
      return wait_for_state.UnwrapError();
    }
    state_ = State::kProcessing;

    Send("@RST\r\n");

    state_ = State::kIdle;
    return ReadResult(1000ms);
  }

  [[nodiscard]]
  types::Result<int, DriverError> InitAllRegister() {
    auto wait_for_state = WaitForState(State::kIdle);
    if (!wait_for_state.IsOk()) {
      return wait_for_state.UnwrapError();
    }
    state_ = State::kProcessing;

    Send("@INI\r\n");

    state_ = State::kIdle;
    return ReadResult(1000ms);
  }

  [[nodiscard]]
  TxState Send(uint8_t address, uint8_t* data, uint32_t length) override {
    auto wait_for_state = WaitForState(State::kIdle);
    if (!wait_for_state.IsOk()) {
      state_ = State::kIdle;
      return TxState::kTimeout;
    }
    state_ = State::kProcessing;

    std::stringstream ss;
    ss << "@TBN";
    ss << std::setw(3) << std::setfill('0') << (int)address;
    ss << std::setw(3) << std::setfill('0') << (int)length;
    for (size_t i = 0; i < length; i++) {
      ss << (char)data[i];
    }
    ss << "\r\n";

    Send(ss.str());

    auto result1 = ReadResult();
    if (!result1.IsOk()) {
      state_ = State::kIdle;
      return TxState::kInvalidResponse;
    }

    if (result1.Unwrap() != +1) {
      logger::Log(logger::Level::kError,
                  "[FEP] \x1b[31mSend\x1b[m Invalid Response: %d",
                  result1.Unwrap());
      state_ = State::kIdle;
      return TxState::kInvalidResponse;
    }

    auto result2 = ReadResult();
    if (!result2.IsOk()) {
      logger::Log(logger::Level::kError,
                  "[FEP] \x1b[31mSend\x1b[m Failed to read result: %s",
                  result2.UnwrapError().c_str());
      state_ = State::kIdle;
      return TxState::kInvalidResponse;
    }

    if (result2.Unwrap() == -10) {
      logger::Log(logger::Level::kError,
                  "[FEP] \x1b[31mSend\x1b[m Invalid Command");
      return TxState::kInvalidCommand;
      state_ = State::kIdle;
      auto result = this->Reset();
      if (!result.IsOk()) {
        logger::Log(logger::Level::kError,
                    "[FEP] \x1b[31mSend\x1b[m Failed to reset: %s",
                    result.UnwrapError().c_str());
        return TxState::kInvalidResponse;

      } else {
        logger::Log(logger::Level::kInfo,
                    "[FEP] \x1b[31mSend\x1b[m Resetted FEP");
      }
      return TxState::kInvalidCommand;
    }
    if (result2.Unwrap() == -1) {
      logger::Log(logger::Level::kError,
                  "[FEP] \x1b[31mSend\x1b[m Failed due to no responce or CS");
      state_ = State::kIdle;
      return TxState::kTimeout;
    }
    if (result2.Unwrap() == -2) {
      logger::Log(logger::Level::kError,
                  "[FEP] \x1b[31mSend\x1b[m Remote RX Overflow");
      state_ = State::kIdle;
      return TxState::kRxOverflow;
    }

    state_ = State::kIdle;
    return TxState::kNoError;
  }
};
}  // namespace fep

using fep::FEP_RawDriver;
}  // namespace robotics::network