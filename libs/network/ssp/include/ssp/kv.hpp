#pragma once

#include <functional>
#include <unordered_map>

#include <logger/logger.hpp>
#include <robotics/network/stream.hpp>
#include <robotics/utils/no_mutex_lifo.hpp>
#include <ssp/ssp.hpp>
#include "robotics/thread/thread.hpp"

namespace robotics::network::ssp {
struct KVPacket {
  uint8_t* data;
  size_t len;
};

template <typename Context = uint8_t>
class KVService : public robotics::network::ssp::SSP_Service<Context> {
  using KVCallback = std::function<KVPacket()>;

  std::unordered_map<uint8_t, KVCallback> kv_callbacks_;

  uint8_t tx_buffer[128] = {};
  utils::NoMutexLIFO<KVPacket, 128> rx_buffer;

 protected:
  void OnKVRequested(uint8_t addr, KVCallback cb) { kv_callbacks_[addr] = cb; }

 private:
  void OnReceive_Request(uint8_t from, uint8_t kv_id) {
    if (kv_callbacks_.find(kv_id) == kv_callbacks_.end()) {
      this->logger.Error("Unknown Key: %d", kv_id);
      return;
    }

    auto res = kv_callbacks_[kv_id]();

    tx_buffer[0] = kv_id;
    for (size_t i = 0; i < res.len; i++) {
      tx_buffer[1 + i] = res.data[i];
    }

    this->Send(from, tx_buffer, res.len + 1);
  }
  void OnReceive_Responce(uint8_t from, KVPacket rx_packet) {
    rx_buffer.Push(rx_packet);
  }

 public:
  KVService(robotics::network::Stream<uint8_t, uint8_t>& stream,
            uint8_t service_id, const char* logger_tag,
            const char* logger_header)
      : SSP_Service<Context>(stream, service_id, logger_tag, logger_header) {
    this->OnReceive([this, &stream](uint8_t addr, uint8_t* data, size_t len) {
      if (len < 1) {
        this->logger.Error("Invalid Length: %d", len);
        return;
      }
      if (len == 1) {
        OnReceive_Request(addr, data[0] << 8);
      } else {
        OnReceive_Responce(addr, {data, len});
      }
    });
  }

  KVPacket GetRemote(uint8_t addr, uint8_t id) {
    using namespace std::chrono_literals;

    tx_buffer[0] = id;

    this->Send(addr, tx_buffer, 1);

    while (true) {
      while (rx_buffer.Empty()) {
        robotics::system::SleepFor(5ms);
      }

      auto res = rx_buffer.Pop();
      if (res.len <= 3) {
        rx_buffer.Push(res);
        continue;
      }

      uint8_t kv_id = res.data[0];
      if (kv_id != id) {
        rx_buffer.Push(res);
        continue;
      }

      return {res.data + 1, res.len - 1};

      robotics::system::SleepFor(2ms);
    }
  }
};
}  // namespace robotics::network::ssp