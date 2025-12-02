#pragma once

#include <functional>
#include <memory>
#include <vector>

#include <robotics/network/can_base.hpp>
#include <robotics/timer/timer.hpp>

using namespace std::chrono_literals;

namespace robotics::network {

class CANModule;

class CANEngine {
 public:
  struct EventCallback {
    using Callback =
        std::function<void(std::uint32_t id, std::vector<uint8_t> const& data)>;

    // accepted when can.id & mask == id
    const std::uint32_t mask;
    const std::uint32_t id;
    Callback cb;

    inline bool Acceptable(std::uint32_t cid) const {
      return (cid & mask) == id;
    }
  };
  using IdleCallback = std::function<void()>;

 private:
  std::shared_ptr<CANBase> can_;
  std::vector<EventCallback> callbacks_;
  uint8_t device_id_;

 public:
  CANEngine(uint8_t device_id, std::shared_ptr<CANBase> can);

  CANEngine(const CANEngine&) = delete;
  CANEngine& operator=(const CANEngine&) = delete;

  void Init() const;
  void RegisterModule(CANModule& mod);

  int Send(uint32_t id, std::vector<uint8_t> const& data) const;
  void OnMessage(std::uint32_t mask, std::uint32_t id,
                 EventCallback::Callback const& cb);

  void OnIdle(IdleCallback const& cb) const;

  int GetDeviceId() const;
};

class CANModule {
  CANModule(const CANModule&) = delete;
  CANModule& operator=(const CANModule&) = delete;

 public:
  CANModule() = default;
  virtual ~CANModule() = default;

  virtual void OnRegister(std::shared_ptr<CANEngine> engine) = 0;
};

namespace can_module {
class Status : public CANModule {
  std::shared_ptr<CANEngine> can_ = nullptr;

 public:
  enum class Statuses : uint8_t {
    kReady = 0x00,
    kCANReady = 0x01,
    kInitializingESC0 = 0x02,
    kInitializingESC1 = 0x03,
    kInitializingGyro = 0x04,
  };

  Status() = default;
  ~Status() override = default;

  void OnRegister(std::shared_ptr<CANEngine> engine) override { can_ = engine; }

  void UpdateStatus(Statuses status) const {
    if (!can_) {
      return;
    }

    std::vector<uint8_t> payload(4);
    payload[0] = 0xf0;
    payload[1] = (this->can_->GetDeviceId() >> 0x08) & 0xff;
    payload[2] = (this->can_->GetDeviceId() >> 0x00) & 0xff;
    payload[3] = static_cast<uint8_t>(status);

    can_->Send(0xa0, payload);
  }
};

class KeepAlive : public CANModule {
 public:
  using KeepAliveLostCallback = std::function<void()>;
  using KeepAliveRecoverdCallback = std::function<void()>;

 private:
  std::shared_ptr<CANEngine> can_;

  robotics::system::Timer keep_alive_timer;
  bool keep_alive_available;

  std::vector<KeepAliveLostCallback> keep_alive_lost_callbacks_;
  std::vector<KeepAliveRecoverdCallback> keep_alive_recovered_callbacks_;

 public:
  ~KeepAlive() override = default;

  void OnRegister(std::shared_ptr<CANEngine> engine) override {
    can_ = engine;
    engine->OnMessage(0x7ff, 0x540,
                      [this](std::uint32_t, std::vector<uint8_t> const&) {
                        keep_alive_timer.Reset();
                      });

    engine->OnIdle([this]() {
      auto timer = keep_alive_timer.ElapsedTime();
      if (keep_alive_available && 300ms < timer) {
        keep_alive_available = false;
        for (auto const& cb : this->keep_alive_lost_callbacks_) {
          cb();
        }
      } else if (!keep_alive_available && timer < 300ms) {
        keep_alive_available = true;
        for (auto const& cb : this->keep_alive_recovered_callbacks_) {
          cb();
        }
      }
    });
  }

  void SendKeepAlive() const { can_->Send(0xfc, {}); }
  void OnKeepAliveLost(KeepAliveLostCallback cb) {
    this->keep_alive_lost_callbacks_.emplace_back(cb);
  }
  void OnKeepAliveRecovered(KeepAliveRecoverdCallback cb) {
    this->keep_alive_recovered_callbacks_.emplace_back(cb);
  }
};

class PingPong : public CANModule {
 public:
  using PongListener = std::function<void(uint8_t device)>;

 private:
  std::shared_ptr<CANEngine> can_;
  std::vector<PongListener> pong_listeners_;

 public:
  ~PingPong() override = default;

  void OnRegister(std::shared_ptr<CANEngine> engine) override {
    can_ = engine;
    engine->OnMessage(0x7f0, 0x530,
                      [this](std::uint32_t, std::vector<uint8_t> const&) {
                        can_->Send(0x81 + can_->GetDeviceId(), {});
                      });

    engine->OnMessage(0x7f0, 0x520,
                      [this](std::uint32_t id, std::vector<uint8_t> const&) {
                        uint8_t device = id & 0x00f;
                        for (auto const& cb : pong_listeners_) {
                          cb(device);
                        }
                      });
  }

  void Ping() const { can_->Send(0x80, {}); }

  void OnPong(PongListener cb) { pong_listeners_.emplace_back(cb); }
};
}  // namespace can_module
}  // namespace robotics::network
