#pragma once

#include <unordered_map>

#include <robotics/logger/logger.hpp>
#include <robotics/network/stream.hpp>
#include <robotics/network/ssp/ssp.hpp>
#include <robotics/node/node.hpp>

namespace robotics::network::ssp {
template <typename Context>
class KeepAliveService : public robotics::network::ssp::SSP_Service<Context> {
  static float constexpr kConnectionLostTime = 1.0; // 10ms
  static float constexpr kKeepAliveInterval = 0.5; // 20ms

  float connection_lost_timer = 0;
  std::unordered_map<Context, float> connection_keeped_timers;

 public:
  Node<bool> connection_available;

  KeepAliveService(robotics::network::Stream<uint8_t, Context>& stream)
      : SSP_Service<Context>(stream, 0x04, "keep_alive.svc.nw",
                             "\x1b[32mKeepAliveService\x1b[m") {
    this->OnReceive([this, &stream](Context addr, uint8_t* data, size_t len) {
      connection_available.SetValue(true);
      connection_lost_timer = kConnectionLostTime;
    });
  }

  void Update(float dt_s) {
    connection_lost_timer -= dt_s;
    if (connection_lost_timer < 0) {
      connection_available.SetValue(false);
    }

    for (auto& [context, timer] : connection_keeped_timers) {
      timer += dt_s;
      if (timer > kKeepAliveInterval) {
        this->Send(context, nullptr, 0);
        timer = 0;
      }
    }
  }

  void AddTarget(Context to) {
    connection_keeped_timers[to] = 0;
  }
};
}  // namespace robotics::network::ssp