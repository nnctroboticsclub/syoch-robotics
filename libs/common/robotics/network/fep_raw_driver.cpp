#include "fep_raw_driver.hpp"

namespace {
robotics::logger::Logger fep_logger{"fep.nw", "\x1b[1;35mFEP\x1b[m"};
robotics::logger::CharLogger rx_logger{
    "sr.fep.nw", "\x1b[1;35mFEP\x1b[m-\x1b[34mCmdRx\x1b[m"};
robotics::logger::CharLogger tx_logger{
    "st.fep.nw", "\x1b[1;35mFEP\x1b[m-\x1b[32mCmdTx\x1b[m"};
}  // namespace

namespace robotics::network::fep {
using namespace std::chrono_literals;

DriverError::DriverError() : std::string() {}
DriverError::DriverError(std::string const& message) : std::string(message) {}

bool DriverResult::Failed() const { return type == Type::kError; }

void FEP_RawDriver::Send(std::string const& data) {
  upper_stream.Send((uint8_t*)data.data(), (uint32_t)data.size());

  for (auto ch : data) {
    tx_logger.Log(ch);
  }
}

void FEP_RawDriver::ISR_ParseBinaryAddress() {
  int addr = (rx_queue_[0] - '0') * 100 + (rx_queue_[1] - '0') * 10 +
             (rx_queue_[2] - '0');

  rx_queue_.Pop();
  rx_queue_.Pop();
  rx_queue_.Pop();

  this->rx_data_address = addr;

  state_ = State::kRxDataLength;
  rx_bytes_need_to_dispatch = 3;
}
void FEP_RawDriver::ISR_ParseBinaryLength() {
  int length = (rx_queue_[0] - '0') * 100 + (rx_queue_[1] - '0') * 10 +
               (rx_queue_[2] - '0');

  rx_queue_.Pop();
  rx_queue_.Pop();
  rx_queue_.Pop();

  rx_data_length = length;

  state_ = State::kRxDataData;
  rx_bytes_need_to_dispatch = length + 2;
}
void FEP_RawDriver::ISR_ParseBinaryData() {
  for (size_t i = 0; i < rx_data_length; i++) {
    on_binary_data_buffer[i] = rx_queue_.Pop();
  }

  rx_queue_.Pop();
  rx_queue_.Pop();

  DispatchOnReceive(rx_data_address, on_binary_data_buffer, rx_data_length);

  state_ = State::kIdle;
  rx_bytes_need_to_dispatch = 0;
}

void FEP_RawDriver::ISR_ParseResult() {
  if (rx_queue_.Size() < 4) {
    return;
  }

  char stat = rx_queue_[0];
  char code = rx_queue_[1];

  if (rx_queue_[2] != '\r') {
    return;
  }
  if (rx_queue_[3] != '\n') {
    return;
  }

  rx_queue_.Pop();
  rx_queue_.Pop();
  rx_queue_.Pop();
  rx_queue_.Pop();

  int value = code - '0';

  DriverResult value_result{
      .type =
          stat == 'P' ? DriverResult::Type::kOk : DriverResult::Type::kError,
      .value = value,
  };

  result_queue_.Push(value_result);

  state_ = State::kIdle;
  rx_bytes_need_to_dispatch = 0;
}
void FEP_RawDriver::ISR_OnUARTData(uint8_t* buffer, uint32_t length) {
  for (size_t i = 0; i < length; i++) {
    rx_logger.Log(buffer[i]);
    if (!rx_queue_.Push(buffer[i])) {
      flags_ |= (uint32_t)Flags::kRxOverflow;
    }
  }

  if (rx_bytes_need_to_dispatch == 0 && rx_queue_.Size()) {
    if (rx_queue_[0] == 'R' && rx_queue_[1] == 'B' && rx_queue_[2] == 'N') {

      rx_queue_.Pop();
      rx_queue_.Pop();
      rx_queue_.Pop();
      state_ = State::kRxDataAddress;
      rx_bytes_need_to_dispatch = 3;
    } else if (rx_queue_[0] == 'N' || rx_queue_[0] == 'P') {

      state_ = State::kRxResult;
      rx_bytes_need_to_dispatch = 4;
    } else {
      return;
    }
  }

  if (rx_bytes_need_to_dispatch > rx_queue_.Size()) {
    return;
  }

  switch (state_) {
    case State::kRxDataAddress:
      ISR_ParseBinaryAddress();
      break;
    case State::kRxDataLength:
      ISR_ParseBinaryLength();
      break;
    case State::kRxDataData:
      ISR_ParseBinaryData();
      break;

    case State::kRxResult:
      ISR_ParseResult();
      break;

    default:
      break;
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

types::Result<std::string, DriverError> FEP_RawDriver::ReadLine(
    std::chrono::milliseconds timeout) {
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
  }
}

FEP_RawDriver::FEP_RawDriver(Stream<uint8_t>& upper_stream)
    : upper_stream(upper_stream) {
  timer_.Reset();
  timer_.Start();
  upper_stream.OnReceive([this](uint8_t* buffer, uint32_t length) {
    ISR_OnUARTData((uint8_t*)buffer, length);
  });
}

types::Result<uint8_t, DriverError> FEP_RawDriver::GetRegister(
    uint8_t address, std::chrono::milliseconds timeout) {
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

}  // namespace robotics::network::fep