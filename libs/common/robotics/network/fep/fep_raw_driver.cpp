#include "fep_raw_driver.hpp"

#include <cstring>

#include "../../platform/thread.hpp"
#include "../block_stream.hpp"

#include "rx_processor.hpp"

namespace {
robotics::logger::Logger fep_logger{"fep.nw", "\x1b[1;35mFEP\x1b[m Gener"};

robotics::logger::CharLogger tx_logger{
    "st.fep.nw", "\x1b[1;35mFEP\x1b[m \x1b[32m====>\x1b[m"};
}  // namespace

namespace robotics::network::fep {
using namespace std::chrono_literals;

DriverError::DriverError() : std::string() {}
DriverError::DriverError(std::string const& message) : std::string(message) {}

bool DriverResult::Failed() const { return type == ResultType::kError; }

void FEP_RawDriver::Send(std::string const& data) {
  upper_stream.Send((uint8_t*)data.data(), (uint32_t)data.size());

  for (auto ch : data) {
    tx_logger.Log(ch);
  }
}

types::Result<int, DriverError> FEP_RawDriver::WaitForState(
    State state, std::chrono::milliseconds timeout) {
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

types::Result<FEPRawLine, DriverError> FEP_RawDriver::ReadLine(
    std::chrono::milliseconds timeout) {
  auto start = timer_.ElapsedTime();

  while (1) {
    if (line_queue_.Empty()) {
      if (timer_.ElapsedTime() > start + timeout) {
        return DriverError("Timeout");
      }
      continue;
    }

    auto line = line_queue_.Pop();
    return line;
  }
}

types::Result<DriverResult, DriverError> FEP_RawDriver::ReadResult(
    std::chrono::milliseconds timeout) {
  auto start = timer_.ElapsedTime();

  while (1) {
    if (!result_queue_.Empty()) {
      return result_queue_.Pop();
    }

    if (timer_.ElapsedTime() > start + timeout) {
      return DriverError("Timeout");
    }

    system::SleepFor(1ms);
  }
}

FEP_RawDriver::FEP_RawDriver(Stream<uint8_t>& upper_stream)
    : upper_stream(upper_stream) {
  rx_processor_ = new RxProcessor();

  timer_.Reset();
  timer_.Start();
  upper_stream.OnReceive([this](uint8_t* buffer, uint32_t length) {
    this->rx_processor_->ISR_OnUARTData(buffer, length);
  });

  this->rx_processor_->OnReceive([this](RxProcessorPacket packet) {
    switch (packet.type) {
      case RxProcessorPacket::Type::kData:
        if (rx_enabled)
          this->DispatchOnReceive(packet.data.from, packet.data.data,
                                  packet.data.length);
        else {
          rx_queue_.Push(packet.data);
        }
        break;
      case RxProcessorPacket::Type::kResult:
        this->result_queue_.Push(packet.result);
        break;
      case RxProcessorPacket::Type::kLine:
        this->line_queue_.Push(packet.line);
        break;
      case RxProcessorPacket::Type::kEnterProcessing:
        this->state_ = State::kProcessing;
        break;
      case RxProcessorPacket::Type::kExitProcessing:
        this->state_ = State::kIdle;
        break;
    }
  });
  fep_thread_.Start([this]() {
    while (1) {
      if (!this->rx_enabled) {
        system::SleepFor(10ms);
        continue;
      }
      if (this->rx_queue_.Empty()) {
        system::SleepFor(1ms);
        continue;
      }

      auto packet = this->rx_queue_.Pop();
      this->DispatchOnReceive(packet.from, packet.data, packet.length);
    }
  });
}

types::Result<uint8_t, DriverError> FEP_RawDriver::GetRegister(
    uint8_t address, std::chrono::milliseconds timeout) {
  auto wait_for_state = WaitForState(State::kIdle, timeout);
  if (!wait_for_state.IsOk()) {
    return wait_for_state.UnwrapError();
  }
  state_ = State::kProcessing;

  std::stringstream ss;
  ss << "@REG" << std::setw(2) << std::setfill('0') << (int)address << "\r\n";
  Send(ss.str());
  ss.str("");

  auto line_res = ReadLine(timeout);
  if (!line_res.IsOk()) {
    return line_res.UnwrapError();
  }

  auto line = line_res.Unwrap();
  int value;
  ss << std::hex << line.line;
  ss >> value;

  state_ = State::kIdle;
  return value;
}

types::Result<DriverResult, DriverError> FEP_RawDriver::SetRegister(
    uint8_t address, uint8_t value) {
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

types::Result<DriverResult, DriverError> FEP_RawDriver::Reset() {
  auto wait_for_state = WaitForState(State::kIdle);
  if (!wait_for_state.IsOk()) {
    return wait_for_state.UnwrapError();
  }
  state_ = State::kProcessing;

  Send("@RST\r\n");

  state_ = State::kIdle;
  return ReadResult(1000ms);
}

void FEP_RawDriver::ResetNoResult() {
  auto wait_for_state = WaitForState(State::kIdle);
  if (!wait_for_state.IsOk()) {
    return;
  }
  state_ = State::kProcessing;

  Send("@RST\r\n");

  state_ = State::kIdle;

  robotics::system::SleepFor(500ms);
  this->FlushQueue();
}

types::Result<DriverResult, DriverError> FEP_RawDriver::InitAllRegister() {
  auto wait_for_state = WaitForState(State::kIdle);
  if (!wait_for_state.IsOk()) {
    return wait_for_state.UnwrapError();
  }
  state_ = State::kProcessing;

  Send("@INI\r\n");

  state_ = State::kIdle;
  return ReadResult(1000ms);
}

TxState FEP_RawDriver::Send(uint8_t address, uint8_t* data, uint32_t length) {
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
    fep_logger.Error("\x1b[31mSend\x1b[m Read1 failed: %s",
                     result1.UnwrapError().c_str());
    state_ = State::kIdle;
    return TxState::kInvalidResponse;
  }

  auto command_result = result1.Unwrap();

  if (command_result.Failed()) {
    fep_logger.Error("\x1b[31mSend\x1b[m Invalid Response: %d", command_result);
    state_ = State::kIdle;
    return TxState::kInvalidResponse;
  }

  auto result2 = ReadResult();
  if (!result2.IsOk()) {
    fep_logger.Error("\x1b[31mSend\x1b[m Failed to read result: %s",
                     result2.UnwrapError().c_str());
    state_ = State::kIdle;
    return TxState::kInvalidResponse;
  }

  auto send_result = result2.Unwrap();

  if (!send_result.Failed()) {
    state_ = State::kIdle;
    return TxState::kNoError;
  }

  switch (send_result.value) {
    case 1: {
      fep_logger.Error("\x1b[31mSend\x1b[m Failed due to no responce or CS");
      state_ = State::kIdle;
      return TxState::kTimeout;
    } break;
    case 2: {
      fep_logger.Error("\x1b[31mSend\x1b[m Remote RX Overflow");
      state_ = State::kIdle;
      return TxState::kRxOverflow;
    }
    case 0:
      fep_logger.Error("\x1b[31mSend\x1b[m Invalid Command");

      [[fallthrough]];
    default: {
      state_ = State::kIdle;
      auto result = this->Reset();
      if (!result.IsOk()) {
        fep_logger.Error("\x1b[31mSend\x1b[m Failed to reset: %s",
                         result.UnwrapError().c_str());
        return TxState::kInvalidResponse;

      } else {
        fep_logger.Info("\x1b[31mSend\x1b[m Resetted FEP");
      }
      return TxState::kInvalidCommand;
    } break;
  }
}

types::Result<FEPRawLine, DriverError> FEP_RawDriver::Version(
    std::chrono::milliseconds timeout) {
  auto wait_for_state = WaitForState(State::kIdle);
  if (!wait_for_state.IsOk()) {
    return wait_for_state.UnwrapError();
  }
  state_ = State::kProcessing;

  Send("\r\n\r\n@VER\r\n");

  auto line_res = ReadLine(timeout);

  state_ = State::kIdle;
  return line_res;
}

void FEP_RawDriver::FlushQueue() {
  result_queue_.Clear();
  rx_queue_.Clear();
  line_queue_.Clear();
}

void FEP_RawDriver::SetDispatchRX(bool enabled) { rx_enabled = enabled; }

}  // namespace robotics::network::fep