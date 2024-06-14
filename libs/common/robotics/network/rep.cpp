#include "rep.hpp"

#include "../logger/logger.hpp"
#include "../platform/random.hpp"

#include <robotics/platform/thread.hpp>

using namespace std::chrono_literals;

namespace {
robotics::logger::Logger rep_logger{"rep.nw", "\x1b[35mREP\x1b[m"};
}

namespace robotics::network::rep {
void ReliableFEPProtocol::_Send(REPTxPacket& packet) {
  if (0) {
    rep_logger.Debug("\x1b[31mSend\x1b[m data = %p (%d B) -> %d", packet.buffer,
                     packet.length, packet.addr);
    rep_logger.Hex(logger::core::Level::kDebug, packet.buffer, packet.length);
  }

  tx_cs_calculator.Reset();
  tx_cs_calculator << (uint8_t)packet.length;
  for (size_t i = 0; i < packet.length; i++) {
    tx_cs_calculator << (uint8_t)packet.buffer[i];
  }

  auto ptr = tx_buffer_;
  *(ptr++) = 0x55;
  *(ptr++) = 0xAA;
  *(ptr++) = 0xCC;
  *(ptr++) = packet.length;
  for (size_t i = 0; i < packet.length; i++) {
    *(ptr++) = packet.buffer[i];
  }

  *(ptr++) = (uint8_t)(tx_cs_calculator.Get() >> 8);
  *(ptr++) = (uint8_t)(tx_cs_calculator.Get() & 0xFF);

  auto tx_state = driver_.Send(packet.addr, tx_buffer_, ptr - tx_buffer_);

  if (tx_state != fep::TxState::kNoError) {
    rep_logger.Error("Failed to send packet to %d: %d, Pushing queue",
                     packet.addr, (int)tx_state);
    tx_queue.Push(packet);
  }

  if (tx_state == fep::TxState::kTimeout) {
    auto duration_ms = int(system::Random::GetByte() / 255.0 * 100);
    rep_logger.Error("Random Backoff: %d ms", duration_ms);
    robotics::system::SleepFor(duration_ms * 1ms);  // random backoff
  }
}

ReliableFEPProtocol::ReliableFEPProtocol(FEP_RawDriver& driver)
    : driver_(driver) {
  driver_.OnReceive([this](uint8_t addr, uint8_t* data, size_t len) {
    //* Validate magic
    if (data[0] != 0x55 || data[1] != 0xAA || data[2] != 0xCC) {
      rep_logger.Error("Invalid Magic: %d", addr);
      return;  // invalid magic
    }

    data += 3;

    //* Load key/length
    if (len <= 4) {
      rep_logger.Error("Invalid Length (1): %d", addr);
      return;  // malformed packet
    }

    uint8_t length = *(data++);
    if (len != (size_t)(6 + length)) {
      rep_logger.Error("Invalid Length (2): %d", addr);
      return;  // malformed packet
    }

    //* Load payload/len
    uint8_t* payload = data;
    uint32_t payload_len = length;
    data += payload_len;

    //* Validate Checksum
    rx_cs_calculator.Reset();
    rx_cs_calculator << (uint8_t)length;
    for (size_t i = 0; i < payload_len; i++) {
      rx_cs_calculator << (uint8_t)payload[i];
    }

    uint16_t checksum = 0;
    checksum |= *(data++) << 8;
    checksum |= *(data++);
    if (checksum != (uint16_t)rx_cs_calculator.Get()) {
      rep_logger.Error("Invalid Checksum: %d", addr);
      return;  // invalid checksum
    }

    if (0) {
      rep_logger.Debug("\x1b[32mRecv\x1b[m data = %p (%d B) -> %d", payload,
                       payload_len, addr);
      rep_logger.Hex(logger::core::Level::kDebug, payload, payload_len);
    }

    DispatchOnReceive(addr, payload, payload_len);
  });

  robotics::system::Thread* thread = new robotics::system::Thread();

  thread->SetStackSize(8192);
  thread->Start([this]() {
    while (1) {
      if (tx_queue.Empty()) {
        robotics::system::SleepFor(100ms);
        continue;
      }

      auto packet = tx_queue.Pop();

      if (0)
        rep_logger.Debug("\x1b[33mSend\x1b[m data = %p (%d B) -> %d",
                         packet.buffer, packet.length, packet.addr);
      _Send(packet);
    }
  });
}

void ReliableFEPProtocol::Send(uint8_t address, uint8_t* data,
                               uint32_t length) {
  static REPTxPacket packet;
  packet = REPTxPacket{address, {}, length};
  for (size_t i = 0; i < length; i++) {
    packet.buffer[i] = data[i];
  }
  tx_queue.Push(packet);
}

}  // namespace robotics::network::rep