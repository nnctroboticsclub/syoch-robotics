#include <robotics/network/fep/fep_raw_driver.hpp>

#include <cstring>

#include <robotics/network/block_stream.hpp>
#include <robotics/thread/thread.hpp>

#include <robotics/network/fep/rx_processor.hpp>

namespace robotics::network::fep {
using namespace std::chrono_literals;

DriverError::DriverError() : std::string() {}
DriverError::DriverError(std::string const& message) : std::string(message) {}

bool DriverResult::Failed() const {
  return type == ResultType::kError;
}

void FEP_RawDriver::Send(std::string const& data) {
  upper_stream.Send((uint8_t*)data.data(), (uint32_t)data.size());
}

types::Result<int, DriverError> FEP_RawDriver::WaitForState(
    State state, std::chrono::milliseconds timeout) {
  auto start = timer_.ElapsedTime();

  while (true) {
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

  while (true) {
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

  while (true) {
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
      using enum robotics::network::fep::RxProcessorPacket::Type;

      case kData:
        if (rx_enabled)
          this->DispatchOnReceive(packet.data.from, packet.data.data.data(),
                                  packet.data.length);
        else {
          rx_queue_.Push(packet.data);
        }
        break;
      case kResult:
        this->result_queue_.Push(packet.result);
        break;
      case kLine:
        this->line_queue_.Push(packet.line);
        break;
      case kEnterProcessing:
        this->state_ = State::kProcessing;
        break;
      case kExitProcessing:
        this->state_ = State::kIdle;
        break;
    }
  });
  fep_thread_.Start([this]() {
    while (true) {
      if (!this->rx_enabled) {
        system::SleepFor(10ms);
        continue;
      }
      if (this->rx_queue_.Empty()) {
        system::SleepFor(1ms);
        continue;
      }

      auto packet = this->rx_queue_.Pop();
      this->DispatchOnReceive(packet.from, packet.data.data(), packet.length);
    }
  });
}

types::Result<uint8_t, DriverError> FEP_RawDriver::GetRegister(
    uint8_t address, std::chrono::milliseconds timeout) {
  if (auto result = WaitForState(State::kIdle, timeout); !result.IsOk()) {
    return result.UnwrapError();
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
  ss << std::hex << line.line.data();
  ss >> value;

  state_ = State::kIdle;
  return value;
}

types::Result<DriverResult, DriverError> FEP_RawDriver::SetRegister(
    uint8_t address, uint8_t value) {
  using enum robotics::network::fep::State;

  if (auto result = WaitForState(kIdle); !result.IsOk()) {
    return result.UnwrapError();
  }
  state_ = kProcessing;

  std::stringstream ss;
  ss << "@REG" << std::setw(2) << std::setfill('0') << (int)address << ":"
     << std::setw(3) << std::setfill('0') << (int)value << "\r\n";
  Send(ss.str());

  state_ = kIdle;
  return ReadResult();
}

types::Result<DriverResult, DriverError> FEP_RawDriver::Reset() {
  using enum robotics::network::fep::State;

  if (auto result = WaitForState(kIdle); !result.IsOk()) {
    return result.UnwrapError();
  }
  state_ = kProcessing;

  Send("@RST\r\n");

  state_ = kIdle;
  return ReadResult(1000ms);
}

void FEP_RawDriver::ResetNoResult() {
  using enum robotics::network::fep::State;

  if (auto result = WaitForState(kIdle); !result.IsOk()) {
    return;
  }
  state_ = kProcessing;

  Send("@RST\r\n");

  state_ = kIdle;

  robotics::system::SleepFor(500ms);
  this->FlushQueue();
}

types::Result<DriverResult, DriverError> FEP_RawDriver::InitAllRegister() {
  using enum robotics::network::fep::State;

  if (auto wait_for_state = WaitForState(kIdle); !wait_for_state.IsOk()) {
    return wait_for_state.UnwrapError();
  }
  state_ = kProcessing;

  Send("@INI\r\n");

  state_ = kIdle;
  return ReadResult(1000ms);
}

TxState FEP_RawDriver::Send(uint8_t address, uint8_t* data, uint32_t length) {
  using enum robotics::network::fep::State;

  if (auto result = WaitForState(State::kIdle); !result.IsOk()) {
    state_ = kIdle;
    return TxState::kTimeout;
  }
  state_ = kProcessing;
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
    state_ = kIdle;
    return TxState::kInvalidResponse;
  }

  if (auto command_result = result1.Unwrap(); command_result.Failed()) {
    state_ = kIdle;
    return TxState::kInvalidResponse;
  }

  auto result2 = ReadResult();
  if (!result2.IsOk()) {
    state_ = kIdle;
    return TxState::kInvalidResponse;
  }

  auto send_result = result2.Unwrap();

  if (!send_result.Failed()) {
    state_ = kIdle;
    return TxState::kNoError;
  }

  switch (send_result.value) {
    case 1: {
      state_ = kIdle;
      return TxState::kTimeout;
    } break;
    case 2: {
      state_ = kIdle;
      return TxState::kRxOverflow;
    }
    case 0:
      [[fallthrough]];
    default: {
      state_ = kIdle;
      if (auto result = this->Reset(); !result.IsOk()) {
        return TxState::kInvalidResponse;
      }

      return TxState::kInvalidCommand;
    } break;
  }
}

types::Result<FEPRawLine, DriverError> FEP_RawDriver::Version(
    std::chrono::milliseconds timeout) {
  using enum robotics::network::fep::State;

  if (auto result = WaitForState(kIdle); !result.IsOk()) {
    return result.UnwrapError();
  }
  state_ = kProcessing;

  Send("\r\n\r\n@VER\r\n");

  state_ = kIdle;
  return ReadLine(timeout);
}

void FEP_RawDriver::FlushQueue() {
  result_queue_.Clear();
  rx_queue_.Clear();
  line_queue_.Clear();
}

void FEP_RawDriver::SetDispatchRX(bool enabled) {
  rx_enabled = enabled;
}

}  // namespace robotics::network::fep