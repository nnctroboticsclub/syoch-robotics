#include <rep.hpp>

#include <logger/logger.hpp>
#include <robotics/random/random.hpp>

#include <robotics/thread/thread.hpp>

using namespace std::chrono_literals;

namespace {
robotics::logger::Logger rep_logger{"rep.nw", "\x1b[35mREP\x1b[m"};
robotics::logger::Logger rep_tx_logger{"tx.rep.nw",
                                       "\x1b[35mREP - \x1b[32mTX\x1b[m"};
robotics::logger::Logger rep_rx_logger{"rx.rep.nw",
                                       "\x1b[35mREP - \x1b[34mRX\x1b[m"};
}  // namespace

namespace robotics::network::rep {
void ReliableFEPProtocol::_Send(REPTxPacket& packet) {
  rep_tx_logger.Debug("\x1b[31mSend\x1b[m data = %p (%d B) -> %d",
                      packet.buffer.data(), packet.length, packet.addr);
  rep_tx_logger.Hex(logger::core::Level::kDebug, packet.buffer.data(),
                    packet.length);

  tx_cs_calculator.Reset();
  for (size_t i = 0; i < packet.length; i++) {
    tx_cs_calculator << (uint8_t)packet.buffer[i];
  }

  auto ptr = tx_buffer_.data();
  *(ptr++) = (uint8_t)(tx_cs_calculator.Get() >> 8);
  *(ptr++) = (uint8_t)(tx_cs_calculator.Get() & 0xFF);

  for (size_t i = 0; i < packet.length; i++) {
    *(ptr++) = packet.buffer[i];
  }

  driver_.Send(packet.addr, tx_buffer_.data(), ptr - tx_buffer_.data());
}

ReliableFEPProtocol::ReliableFEPProtocol(FEP_RawDriver& driver)
    : driver_(driver) {
  driver_.OnReceive([this](uint8_t addr, uint8_t* data, size_t len) {
    in_isr = true;
    //* Load Checksum
    uint16_t remote_checksum = data[0] << 8 | data[1];

    auto payload_len = len - 2;
    uint8_t* payload = data + 2;

    //* Validate Checksum
    rx_cs_calculator.Reset();
    for (size_t i = 0; i < payload_len; i++) {
      rx_cs_calculator << payload[i];
    }

    if (auto local_checksum = rx_cs_calculator.Get();
        local_checksum != remote_checksum) {
      rep_logger.Error("Checksum Mismatch: @%d local(%d)!=remote(%d)", addr,
                       local_checksum, remote_checksum);
      rep_logger.Hex(logger::core::Level::kError, payload, payload_len);
      in_isr = false;
      return;
    }

    DispatchOnReceive(addr, payload, payload_len);
    in_isr = false;
  });

  auto* thread = new robotics::system::Thread();

  thread->SetStackSize(8192);
  thread->Start([this]() {
    while (true) {
      if (tx_queue.Empty()) {
        robotics::system::SleepFor(1ms);
        continue;
      }

      auto packet = tx_queue.Pop();

      _Send(packet);
    }
  });
}

void ReliableFEPProtocol::Send(uint8_t address, uint8_t* data,
                               uint32_t length) {
  static REPTxPacket packet;
  packet = REPTxPacket{.addr = address, .buffer = {}, .length = length};
  for (size_t i = 0; i < length; i++) {
    packet.buffer[i] = data[i];
  }

  if (!in_isr) {
    _Send(packet);
  } else {
    tx_queue.Push(packet);
  }
}

}  // namespace robotics::network::rep