#pragma once

#include "rx_processor_packet.hpp"

#include <array>
#include <robotics/network/block_stream.hpp>

#include <logger/logger.hpp>
#include <robotics/utils/no_mutex_lifo.hpp>

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

  std::array<uint8_t, 128> on_binary_data_buffer_;

  inline void EnterProcessing() {
    auto message = RxProcessorPacket::EnterProcessing();

    DispatchOnReceive(message);
    UpdateState(RxState::kDetecting);
  }

  inline void ExitProcessing() {
    auto message = RxProcessorPacket::ExitProcessing();
    DispatchOnReceive(message);
    UpdateState(RxState::kNone);
  }

  inline void UpdateState(RxState new_state) {
    using enum RxState;
    state_ = new_state;

    switch (state_) {
      case kDataHeader:
        need_to_dispatch_ = 6;
        break;
      case kDataContent:
        need_to_dispatch_ = rx_data_length_ + 2;
        break;
      case kResult:
        need_to_dispatch_ = 4;
        break;
      case kNone:
      case kDetecting:
        need_to_dispatch_ = 0;
        break;
      case kRawLine:
        break;
    }
  }

 public:
  RxProcessor() = default;

  void Send(RxProcessorPacket&) override {
    // Do nothing
  }

  inline void ISR_ParseBinaryHeader() {
    int addr = (rx_queue_[0] - '0') * 100 + (rx_queue_[1] - '0') * 10 +
               (rx_queue_[2] - '0');

    int length = (rx_queue_[3] - '0') * 100 + (rx_queue_[4] - '0') * 10 +
                 (rx_queue_[5] - '0');

    rx_queue_.ConsumeN(6);

    rx_data_address_ = static_cast<uint8_t>(addr);
    rx_data_length_ = static_cast<uint8_t>(length);

    UpdateState(RxState::kDataContent);
  }

  inline void ISR_ParseBinaryData() {
    rx_queue_.PopNTo(rx_data_length_, (char*)on_binary_data_buffer_.data());
    rx_queue_.ConsumeN(2);

    auto data = FEPPacket{
        .from = rx_data_address_,
        .length = rx_data_length_,
    };
    memcpy(data.data.data(), on_binary_data_buffer_.data(), rx_data_length_);

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

    auto message = RxProcessorPacket::Result(value_result);
    DispatchOnReceive(message);

    ExitProcessing();
  }

  inline void ISR_ParseRawLine() {
    std::array<char, 64> line{};
    auto len = rx_queue_.Size();

    rx_queue_.PopNTo(len, line.data());

    line[len] = '\0';

    auto message = RxProcessorPacket::Line(line.data());
    DispatchOnReceive(message);

    ExitProcessing();
  }

  inline void ISR_OnUARTData(uint8_t* buffer, uint32_t length) {
    using enum robotics::network::fep::RxProcessor::RxState;
    if (!rx_queue_.PushN((char*)buffer, length)) {
      return;
    }

    if (this->state_ == kNone) {
      EnterProcessing();
    }

    if (this->state_ == kDetecting) {
      if (rx_queue_.Size() >= 3 && rx_queue_[0] == 'R' && rx_queue_[1] == 'B' &&
          rx_queue_[2] == 'N') {
        rx_queue_.ConsumeN(3);
        UpdateState(kDataHeader);
      } else if (rx_queue_.Size() >= 2 &&
                 (rx_queue_[0] == 'N' || rx_queue_[0] == 'P')) {
        UpdateState(kResult);
      } else if (rx_queue_.Size() >= 3) {
        UpdateState(kRawLine);
        return;
      }
    }

    if (state_ == kRawLine) {
      if (buffer[0] == '\n') {
        ISR_ParseRawLine();
        UpdateState(kNone);
      }
      return;
    }

    if (rx_queue_.Size() < need_to_dispatch_) {
      return;
    }

    switch (state_) {
      case kDataHeader:
        ISR_ParseBinaryHeader();
        break;

      case kDataContent:
        ISR_ParseBinaryData();
        break;

      case kResult:
        ISR_ParseResult();
        break;

      default:
        break;
    }
  }
};
}  // namespace robotics::network::fep