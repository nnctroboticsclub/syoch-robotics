#pragma once

#include <unordered_map>
#include <functional>

#include <robotics/logger/logger.hpp>
#include <robotics/network/stream.hpp>
#include <robotics/network/ssp/ssp.hpp>
#include <robotics/platform/thread.hpp>
#include <robotics/utils/no_mutex_lifo.hpp>

namespace robotics::network::ssp {
struct KVPacket {
  uint8_t* data;
  size_t len;
};

class KVService : public robotics::network::ssp::SSP_Service {
  using KVCallback = std::function<KVPacket()>;

  std::unordered_map<uint16_t, KVCallback> kv_callbacks_;

  uint8_t tx_buffer[128] = {};
  utils::NoMutexLIFO<KVPacket, 128> rx_buffer;

 protected:
  void OnKVRequested(uint8_t addr, KVCallback cb) { kv_callbacks_[addr] = cb; }

 private:
  void OnReceive_Request(uint8_t from, uint16_t kv_id) {
    if (kv_callbacks_.find(kv_id) == kv_callbacks_.end()) {
      logger.Error("Unknown Key: %d", kv_id);
      return;
    }

    auto res = kv_callbacks_[kv_id]();

    tx_buffer[0] = (kv_id >> 8) & 0xFF;
    tx_buffer[1] = kv_id & 0xFF;
    for (size_t i = 0; i < res.len; i++) {
      tx_buffer[2 + i] = res.data[i];
    }

    this->Send(from, tx_buffer, res.len + 2);
  }
  void OnReceive_Responce(uint8_t from, KVPacket rx_packet) {
    rx_buffer.Push(rx_packet);
  }

 public:
  KVService(robotics::network::Stream<uint8_t, uint8_t>& stream,
            uint16_t service_id, const char* logger_tag,
            const char* logger_header)
      : SSP_Service(stream, service_id, logger_tag, logger_header) {
    OnReceive([this, &stream](uint8_t addr, uint8_t* data, size_t len) {
      if (len < 2) {
        logger.Error("Invalid Length: %d", len);
        return;
      }
      if (len == 2) {
        OnReceive_Request(addr, (data[0] << 8) | data[1]);
      } else {
        OnReceive_Responce(addr, {data, len});
      }
    });
  }

  KVPacket GetRemote(uint8_t addr, uint16_t id) {
    using namespace std::chrono_literals;

    tx_buffer[0] = (id >> 8) & 0xFF;
    tx_buffer[1] = id & 0xFF;

    this->Send(addr, tx_buffer, 2);

    while (true) {
      while (rx_buffer.Empty()) {
        robotics::system::SleepFor(5ms);
      }

      auto res = rx_buffer.Pop();
      if (res.len <= 3) {
        rx_buffer.Push(res);
        continue;
      }

      uint16_t kv_id = (res.data[0] << 8) | res.data[1];
      if (kv_id != id) {
        rx_buffer.Push(res);
        continue;
      }

      return {res.data + 2, res.len - 2};

      robotics::system::SleepFor(2ms);
    }
  }
};
}  // namespace robotics::network::ssp