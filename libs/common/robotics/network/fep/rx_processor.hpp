#pragma once

#include "rx_processor_packet.hpp"

#include "../block_stream.hpp"

#include <robotics/utils/no_mutex_lifo.hpp>
#include <robotics/logger/logger.hpp>

namespace {
robotics::logger::Logger fep_rxp_logger{"rxp.fep.nw",
                                        "\x1b[1;35mFEP RxPro\x1b[m"};
robotics::logger::CharLogger rx_logger{
    "sr.fep.nw", "\x1b[1;35mFEP\x1b[m \x1b[34m<====\x1b[m"};
}  // namespace

namespace robotics::network::fep {
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

  inline void FlushRxLog() {
    rx_logger.Flush();
    rx_logger.ClearBuffer();
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

    if (state_ == RxState::kNone && rx_logger.CacheAvailable()) {
      fep_rxp_logger.Debug("There is remaining bytes in rx_log");
      FlushRxLog();
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
    FlushRxLog();

    rx_queue_.ConsumeN(6);

    rx_data_address_ = addr;
    rx_data_length_ = length;

    UpdateState(RxState::kDataContent);
  }

  inline void ISR_ParseBinaryData() {
    FlushRxLog();
    rx_queue_.PopNTo(rx_data_length_, (char*)on_binary_data_buffer_);
    rx_queue_.ConsumeN(2);

    fep_rxp_logger.Debug("Dispatching %d bytes from %d", rx_data_length_,
                         rx_data_address_);

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

    FlushRxLog();
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
    char line[64] = {0};
    auto len = rx_queue_.Size();

    fep_rxp_logger.Debug("Raw line: %d bytes", len);
    FlushRxLog();

    rx_queue_.PopNTo(len, line);

    line[len] = '\0';

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
}  // namespace robotics::network::fep