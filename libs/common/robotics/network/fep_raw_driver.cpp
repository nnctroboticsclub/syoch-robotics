#include "fep_raw_driver.hpp"

#include <cstring>

#include "../platform/thread.hpp"
#include "block_stream.hpp"

namespace {
robotics::logger::Logger fep_rxp_logger{"rxp.fep.nw",
                                        "\x1b[1;35mFEP RxPro\x1b[m"};

robotics::logger::Logger fep_logger{"fep.nw", "\x1b[1;35mFEP\x1b[m Gener"};
robotics::logger::CharLogger rx_logger{
    "sr.fep.nw", "\x1b[1;35mFEP\x1b[m \x1b[34m<====\x1b[m"};
robotics::logger::CharLogger tx_logger{
    "st.fep.nw", "\x1b[1;35mFEP\x1b[m \x1b[32m====>\x1b[m"};
}  // namespace

namespace robotics::network::fep {
using namespace std::chrono_literals;

DriverError::DriverError() : std::string() {}
DriverError::DriverError(std::string const& message) : std::string(message) {}

bool DriverResult::Failed() const { return type == ResultType::kError; }

struct RxProcessorPacket {
  enum class Type : uint8_t {
    kData,
    kResult,
    kLine,
    kEnterProcessing,
    kExitProcessing,
  };

  Type type;

  union {
    FEPPacket data;
    DriverResult result;
    FEPRawLine line;
  };

  static RxProcessorPacket Data(FEPPacket fep_packet) {
    RxProcessorPacket packet;
    packet.type = Type::kData;
    packet.data = fep_packet;
    return packet;
  }

  static RxProcessorPacket Result(DriverResult result) {
    RxProcessorPacket packet;
    packet.type = Type::kResult;
    packet.result = result;
    return packet;
  }

  static RxProcessorPacket Line(char const* line) {
    RxProcessorPacket packet;
    packet.type = Type::kLine;
    strncpy(packet.line.line, line, 64);
    return packet;
  }

  static RxProcessorPacket EnterProcessing() {
    RxProcessorPacket packet;
    packet.type = Type::kEnterProcessing;
    return packet;
  }

  static RxProcessorPacket ExitProcessing() {
    RxProcessorPacket packet;
    packet.type = Type::kExitProcessing;
    return packet;
  };
};

class RxProcessor : public BlockStream<RxProcessorPacket> {
  enum class RxState {
    kNone,
    kDetecting,

    kDataHeader,
    kDataContent,

    kRawLine,

    kResult,
  };

  robotics::utils::NoMutexLIFO<char, 64> rx_queue_;

  uint8_t rx_data_address_ = 0;
  uint8_t rx_data_length_ = 0;

  RxState state_ = RxState::kNone;
  size_t need_to_dispatch_ = 0;

  uint8_t on_binary_data_buffer_[128];

  inline void EnterProcessing() {
    fep_rxp_logger.Debug("<========");
    auto message = RxProcessorPacket::EnterProcessing();

    DispatchOnReceive(message);
    UpdateState(RxState::kDetecting);
  }

  inline void ExitProcessing() {
    fep_rxp_logger.Debug("========>");
    auto message = RxProcessorPacket::ExitProcessing();
    DispatchOnReceive(message);
    UpdateState(RxState::kNone);
  }

  inline void UpdateState(RxState new_state) {
    /* fep_rxp_logger.Debug("Entering state = %d\n", (int)new_state); */
    state_ = new_state;

    switch (state_) {
      case RxState::kDataHeader:
        need_to_dispatch_ = 6;
        break;
      case RxState::kDataContent:
        need_to_dispatch_ = rx_data_length_ + 2;
        break;
      case RxState::kResult:
        need_to_dispatch_ = 4;
        break;
      case RxState::kNone:
      case RxState::kDetecting:
        need_to_dispatch_ = 0;
        break;
    }

    if (state_ != RxState::kNone && state_ != RxState::kDetecting) {
      rx_logger.Flush();
      rx_logger.ClearBuffer();
    }
  }

 public:
  RxProcessor() {
    rx_logger.SetCachingMode(robotics::logger::CharLogger::CachingMode::kUser);
    rx_logger.SetHeader("    ||| ");
  }
  void Send(RxProcessorPacket&) override {
    fep_rxp_logger.Error("Send is not supported");
  }

  inline void ISR_ParseBinaryHeader() {
    int addr = (rx_queue_[0] - '0') * 100 + (rx_queue_[1] - '0') * 10 +
               (rx_queue_[2] - '0');

    int length = (rx_queue_[3] - '0') * 100 + (rx_queue_[4] - '0') * 10 +
                 (rx_queue_[5] - '0');

    fep_rxp_logger.Debug("Data: #%d+%d, from '%c%c%c%c%c%c'", addr, length,
                         rx_queue_[0], rx_queue_[1], rx_queue_[2], rx_queue_[3],
                         rx_queue_[4], rx_queue_[5]);

    rx_queue_.ConsumeN(6);

    rx_data_address_ = addr;
    rx_data_length_ = length;

    UpdateState(RxState::kDataContent);
  }

  inline void ISR_ParseBinaryData() {
    rx_queue_.PopNTo(rx_data_length_, (char*)on_binary_data_buffer_);
    rx_queue_.ConsumeN(2);

    fep_rxp_logger.Debug("Dispatching %d bytes from %d", rx_data_length_,
                         rx_data_address_);
    fep_rxp_logger.Hex(robotics::logger::core::Level::kDebug,
                       on_binary_data_buffer_, rx_data_length_);

    auto data = FEPPacket{
        .from = rx_data_address_,
        .length = rx_data_length_,
    };
    memcpy(data.data, on_binary_data_buffer_, rx_data_length_);

    auto message = RxProcessorPacket::Data(data);
    DispatchOnReceive(message);

    ExitProcessing();
  }

  inline void ISR_ParseResult() {
    char stat = rx_queue_[0];
    char code = rx_queue_[1];

    if (rx_queue_[2] != '\r') {
      return;
    }
    if (rx_queue_[3] != '\n') {
      return;
    }

    rx_queue_.ConsumeN(4);

    int value = code - '0';

    DriverResult value_result{
        .type = stat == 'P' ? ResultType::kOk : ResultType::kError,
        .value = value,
    };

    fep_rxp_logger.Debug("Result: %c%d", stat, value);

    auto message = RxProcessorPacket::Result(value_result);
    DispatchOnReceive(message);

    ExitProcessing();
  }

  inline void ISR_ParseRawLine() {
    char line[64];
    rx_queue_.PopAllTo(line);

    auto message = RxProcessorPacket::Line(line);
    DispatchOnReceive(message);

    ExitProcessing();
  }

  inline void ISR_OnUARTData(uint8_t* buffer, uint32_t length) {
    rx_logger.LogN((char*)buffer, length);
    if (!rx_queue_.PushN((char*)buffer, length)) {
      fep_rxp_logger.Error("RX Overflow");
      return;
    }

    if (this->state_ == RxState::kNone) {
      EnterProcessing();
    }

    if (this->state_ == RxState::kDetecting) {
      if (rx_queue_.Size() >= 3 && rx_queue_[0] == 'R' && rx_queue_[1] == 'B' &&
          rx_queue_[2] == 'N') {
        rx_queue_.ConsumeN(3);
        UpdateState(RxState::kDataHeader);
      } else if (rx_queue_.Size() >= 2 && rx_queue_[0] == 'N' ||
                 rx_queue_[0] == 'P') {
        UpdateState(RxState::kResult);
      } else if (rx_queue_.Size() >= 3) {
        UpdateState(RxState::kRawLine);
        return;
      }
    }

    if (state_ == RxState::kRawLine) {
      if (buffer[0] == '\n') {
        ISR_ParseRawLine();
        UpdateState(RxState::kNone);
      }
      return;
    }

    if (rx_queue_.Size() < need_to_dispatch_) {
      return;
    }

    switch (state_) {
      case RxState::kDataHeader:
        ISR_ParseBinaryHeader();
        break;

      case RxState::kDataContent:
        ISR_ParseBinaryData();
        break;

      case RxState::kResult:
        ISR_ParseResult();
        break;

      default:
        break;
    }
  }
};

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
        this->DispatchOnReceive(packet.data.from, packet.data.data,
                                packet.data.length);
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