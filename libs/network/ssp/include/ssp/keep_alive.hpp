#pragma once

#include <unordered_map>

#include <robotics/network/stream.hpp>
#include <robotics/node/node.hpp>

#include <logger/logger.hpp>
#include <ssp/ssp.hpp>

namespace robotics::network::ssp {
template <typename Context, typename TxRet = void>
class KeepAliveService
    : public robotics::network::ssp::SSP_Service<Context, TxRet> {
  static float constexpr kConnectionLostTime = 0.4;  // 10ms

  float connection_lost_timer = 0;
  std::unordered_map<Context, float> connection_keeped_timers;

 public:
  Node<bool> connection_available;

  KeepAliveService(robotics::network::Stream<uint8_t, Context, TxRet>& stream)
      : SSP_Service<Context, TxRet>(stream, 0x04, "keep_alive.svc.nw",
                                    "\x1b[32mKeepAliveService\x1b[m") {
    this->OnReceive(
        [this](Context addr, uint8_t* data, size_t len) { TreatKeepAlive(); });
  }

  void Update(float dt_s) {
    connection_lost_timer -= dt_s;
    if (connection_lost_timer < 0) {
      connection_available.SetValue(false);
    }

    for (auto& [context, timer] : connection_keeped_timers) {
      timer += dt_s;
    }
  }

  void TreatKeepAlive() {
    connection_available.SetValue(true);
    connection_lost_timer = kConnectionLostTime;
  }

  void SendKeepAliveToAll() {
    for (auto& [context, timer] : connection_keeped_timers) {
      this->Send(context, nullptr, 0);
      timer = 0;
    }
  }

  void SendKeepAliveTo(Context to) {
    connection_keeped_timers[to] = 0;
    this->Send(to, nullptr, 0);
  }

  void AddTarget(Context to) { connection_keeped_timers[to] = 0; }
};
}  // namespace robotics::network::ssp