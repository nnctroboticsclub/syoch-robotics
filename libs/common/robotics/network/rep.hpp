#pragma once

#include <cstdint>

#include "checksum.hpp"
#include "stream.hpp"
#include "fep_tx_state.hpp"

#include "../utils/no_mutex_lifo.hpp"
#include "../platform/timer.hpp"
#include "../platform/random.hpp"
#include "../logger/logger.hpp"

namespace robotics::network {
namespace rep {
struct REPTxPacket {
  uint8_t no_key_check;
  uint8_t addr;
  uint8_t buffer[32];
  uint32_t length;
};

using namespace std::chrono_literals;
class CachedKey {
  static inline system::Timer key_request_timer;

  enum class State : uint8_t {
    kUnknown,
    kInitialized,
    kWaiting
  } state = State::kUnknown;
  std::chrono::microseconds request_queued_at;  // runtime timestamp
  uint8_t key = 0;

 public:
  void ResetState() {
    state = State::kUnknown;
    request_queued_at = 0s;
    key = 0;
  }

  void InitializedState(uint8_t key) {
    this->key = key;
    this->state = State::kInitialized;
  }

  void RequestState() {
    this->state = State::kWaiting;
    this->request_queued_at = key_request_timer.ElapsedTime();
    this->key = 0;
  }

  bool IsNeedRequest() const {
    if (state == State::kUnknown) return true;
    if (state == State::kInitialized) return false;

    if (key_request_timer.ElapsedTime() > request_queued_at + 1s) return true;
  }

  bool IsReady() { return state == State::kInitialized; }

  uint8_t GetKey() const { return key; }

  const char* c_str() const {
    static char buffer[32];
    snprintf(buffer, sizeof(buffer), "%d(%d)", key, (uint8_t)state);
    return buffer;
  }
};

class ReliableFEPProtocol : public Stream<uint8_t, uint8_t> {
  Stream<uint8_t, uint8_t, fep::TxState>& driver_;

  uint8_t random_key_;

  Checksum rx_cs_calculator;
  Checksum tx_cs_calculator;

  uint8_t tx_buffer_[32] = {};
  CachedKey cached_keys[256] = {};  // Using array for ISR context

  robotics::utils::NoMutexLIFO<REPTxPacket, 4> tx_queue;

  void ExchangeKey_RequestImmediately(uint8_t remote) {
    logger::Log(logger::Level::kVerbose, "[REP] Key Request: to %d (Imm)",
                remote);

    REPTxPacket* packet =
        new REPTxPacket{true, remote, {system::Random::GetByte()}, 1};
    _Send(*packet);

    delete packet;

    cached_keys[remote].RequestState();
  }
  void ExchangeKey_Request(uint8_t remote) {
    logger::Log(logger::Level::kVerbose, "[REP] Key Request: to %d", remote);

    this->tx_queue.Push({true, remote, {system::Random::GetByte()}, 1});

    cached_keys[remote].RequestState();
  }

  void ExchangeKey_Responce(uint8_t remote, uint8_t random) {
    this->tx_queue.Push(
        {true, remote, {random, (uint8_t)(random ^ random_key_)}, 2});

    logger::Log(logger::Level::kVerbose,
                "[REP] Key Responcing: for %d, random=%#02x", remote, random);
  }

  void ExchangeKey_Update(uint8_t remote, uint8_t key) {
    cached_keys[remote].InitializedState(key);

    logger::Log(logger::Level::kVerbose, "[REP] Key Updated: for %d, key=%#02x",
                remote, key);
  }

  void _Send(REPTxPacket& packet) {
    if (!packet.no_key_check && cached_keys[packet.addr].IsNeedRequest()) {
      logger::Log(logger::Level::kDebug,
                  "[REP] Packet marked as key_check, but the cached key is not "
                  "initialized for %d, Queueing exchanging...",
                  packet.addr);
      ExchangeKey_Request(packet.addr);

      return;
    }

    if (0) {
      logger::Log(logger::Level::kDebug,
                  "[REP] \x1b[31mSend\x1b[m key = %s, data = %p (%d B) -> %d",
                  cached_keys[packet.addr].c_str(), packet.buffer,
                  packet.length, packet.addr);
      logger::LogHex(logger::Level::kDebug, packet.buffer, packet.length);
    }

    auto key = cached_keys[packet.addr].GetKey();

    tx_cs_calculator.Reset();
    tx_cs_calculator << (uint8_t)key;
    tx_cs_calculator << (uint8_t)packet.length;
    for (size_t i = 0; i < packet.length; i++) {
      tx_cs_calculator << (uint8_t)packet.buffer[i];
    }

    auto ptr = tx_buffer_;
    *(ptr++) = 0x55;
    *(ptr++) = 0xAA;
    *(ptr++) = 0xCC;
    *(ptr++) = key;
    *(ptr++) = packet.length;
    for (size_t i = 0; i < packet.length; i++) {
      *(ptr++) = packet.buffer[i];
    }

    *(ptr++) = (uint8_t)(tx_cs_calculator.Get() >> 8);
    *(ptr++) = (uint8_t)(tx_cs_calculator.Get() & 0xFF);

    auto tx_state = driver_.Send(packet.addr, tx_buffer_, ptr - tx_buffer_);

    if (tx_state != fep::TxState::kNoError) {
      /* logger::Log(logger::Level::kError,
                  "[REP] Failed to send packet to %d: %d, Pushing queue",
                  packet.addr, (int)tx_state); */
      tx_queue.Push(packet);
    }

    if (tx_state == fep::TxState::kTimeout) {
      auto duration_ms = int(system::Random::GetByte() / 255.0 * 50);
      logger::Log(logger::Level::kError,
                  "[REP] Random Backoff: %d ms",
                  duration_ms);
      ThisThread::sleep_for(
         duration_ms* 1ms);  // random backoff

    }
  }

 public:
  ReliableFEPProtocol(FEP_RawDriver& driver) : driver_(driver) {
    this->random_key_ = system::Random::GetByte();

    logger::Log(logger::Level::kInfo, "[REP] Using Random Key: \e[1;32m%d\e[m",
                random_key_);

    driver_.OnReceive([this](uint8_t addr, uint8_t* data, size_t len) {
      //* Validate magic
      if (data[0] != 0x55 || data[1] != 0xAA || data[2] != 0xCC) {
        logger::Log(logger::Level::kError, "[REP] Invalid Magic: %d", addr);
        return;  // invalid magic
      }

      data += 3;

      //* Load key/length
      if (len <= 5) {
        logger::Log(logger::Level::kError, "[REP] Invalid Length (1): %d",
                    addr);
        return;  // malformed packet
      }

      uint8_t key = *(data++);
      uint8_t length = *(data++);
      if (len != 7 + length) {
        logger::Log(logger::Level::kError, "[REP] Invalid Length (2): %d",
                    addr);
        return;  // malformed packet
      }

      //* Load payload/len
      uint8_t* payload = data;
      uint32_t payload_len = length;
      data += payload_len;

      //* Validate Checksum
      rx_cs_calculator.Reset();
      rx_cs_calculator << (uint8_t)key;
      rx_cs_calculator << (uint8_t)length;
      for (size_t i = 0; i < payload_len; i++) {
        rx_cs_calculator << (uint8_t)payload[i];
      }

      uint16_t checksum = 0;
      checksum |= *(data++) << 8;
      checksum |= *(data++);
      if (checksum != (uint16_t)rx_cs_calculator.Get()) {
        logger::Log(logger::Level::kError, "[REP] Invalid Checksum: %d", addr);
        return;  // invalid checksum
      }

      if (0) {
        logger::Log(logger::Level::kDebug,
                    "[REP] \x1b[32mRecv\x1b[m key = %d, data = %p (%d B) -> %d",
                    key, payload, payload_len, addr);
        logger::LogHex(logger::Level::kDebug, payload, payload_len);
      }

      if (key == 0) {
        if (payload_len == 1)  // key request
        {
          ExchangeKey_Responce(addr, payload[0]);
          return;
        } else if (payload_len == 2)  // key responce
        {
          ExchangeKey_Update(addr, payload[0] ^ payload[1]);
          return;
        }
      }

      if (key != random_key_) {
        logger::Log(logger::Level::kError, "[REP] Invalid Key: %d", addr);
        ExchangeKey_Responce(addr, system::Random::GetByte());
        return;  // invalid key
      }

      DispatchOnReceive(addr, payload, payload_len);
    });

    (new Thread(osPriorityNormal, 8192, nullptr, "REP"))->start([this]() {
      while (1) {
        if (tx_queue.Empty()) {
          ThisThread::sleep_for(100ms);
          continue;
        }

        auto packet = tx_queue.Pop();

        if (!packet.no_key_check && !cached_keys[packet.addr].IsReady()) {
          if (cached_keys[packet.addr].IsNeedRequest()) {
            ExchangeKey_Request(packet.addr);
          }

          if (0)
            logger::Log(
                logger::Level::kVerbose,
                "[REP] \x1b[35mSend\x1b[m key = %s, data = %p (%d B) -> %d",
                cached_keys[packet.addr].c_str(), packet.buffer, packet.length,
                packet.addr);
          tx_queue.Push(packet);

          continue;
        }

        if (0)
          logger::Log(
              logger::Level::kDebug,
              "[REP] \x1b[33mSend\x1b[m key = %s, data = %p (%d B) -> %d",
              cached_keys[packet.addr].c_str(), packet.buffer, packet.length,
              packet.addr);
        _Send(packet);
      }
    });
  }

  void Send(uint8_t address, uint8_t* data, uint32_t length) override {
    static REPTxPacket packet;
    packet = REPTxPacket{false, address, {}, length};
    for (size_t i = 0; i < length; i++) {
      packet.buffer[i] = data[i];
    }
    tx_queue.Push(packet);
  }
};
}  // namespace rep
using ReliableFEPProtocol = rep::ReliableFEPProtocol;
}  // namespace robotics::network