#pragma once

#include <unordered_map>
#include <functional>
#include <vector>
#include <iomanip>
#include <memory>

#include "can_base.hpp"

#include "../platform/timer.hpp"

namespace robotics::network {
class DistributedCAN {
 public:
  enum class Statuses : uint8_t {
    kReady = 0x00,
    kCANReady = 0x01,
    kInitializingESC0 = 0x02,
    kInitializingESC1 = 0x03,
    kInitializingGyro = 0x04,
  };

  struct EventCallback {
    using Callback =
        std::function<void(std::uint32_t id, std::vector<uint8_t> const &data)>;

    // accepted when can.id & mask == id
    const std::uint32_t mask;
    const std::uint32_t id;
    Callback cb;

    inline bool Acceptable(std::uint32_t cid) const {
      return (cid & mask) == id;
    }
  };

  using KeepAliveLostCallback = std::function<void()>;
  using KeepAliveRecoverdCallback = std::function<void()>;

  using PongListener = std::function<void(uint8_t device)>;

 private:
  int can_id = 0x7;
  std::shared_ptr<CANBase> can_;
  int last_keep_alive = 0;
  robotics::system::Timer keep_alive_timer;

  std::vector<EventCallback> callbacks_;

  bool keep_alive_available;
  std::vector<KeepAliveLostCallback> keep_alive_lost_callbacks_;
  std::vector<KeepAliveRecoverdCallback> keep_alive_recovered_callbacks_;

  std::vector<PongListener> pong_listeners_;

  inline void HandleMessage(std::uint32_t id, std::vector<uint8_t> const &data);

 public:
  DistributedCAN(int can_id, std::shared_ptr<CANBase> can);
  void Init();

  void OnMessage(std::uint32_t mask, std::uint32_t id,
                 EventCallback::Callback cb);

  CANBase::Capability GetCapability();
  float GetBusLoad();

  void OnRx(CANBase::RxCallback cb);
  void OnTx(CANBase::TxCallback cb);
  void OnIdle(CANBase::IdleCallback cb);

  void SendKeepAlive();
  void OnKeepAliveLost(KeepAliveLostCallback cb);
  void OnKeepAliveRecovered(KeepAliveRecoverdCallback cb);

  int Send(uint8_t element_id, std::vector<uint8_t> const &data);
  void SetStatus(Statuses status);

  void Ping();
  void OnPong(PongListener cb);

  int GetCanId() { return can_id; }
};
}  // namespace robotics::network