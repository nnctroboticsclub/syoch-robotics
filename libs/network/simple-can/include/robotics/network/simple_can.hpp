#pragma once

#include <NanoHW/parallel.hpp>
#include <NanoHW/policies.hpp>
#include <NanoHW/thread.hpp>
#include <vector>

#include <mbed.h>

#include <Nano/non_copyable.hpp>
#include <NanoHW/can.hpp>
#include <robotics/network/can_base.hpp>

namespace robotics::network {
class SimpleCAN : public CANBase, public Nano::utils::NonCopyable<SimpleCAN> {
  struct Context {
    std::vector<RxCallback> rx_callbacks{};
    std::vector<TxCallback> tx_callbacks{};
    void OnCANReceived(nano_hw::can::CANMessage const& msg) {
      auto id = msg.id;
      std::vector<uint8_t> data(msg.data, msg.data + msg.len);

      for (auto& cb : rx_callbacks) {
        cb(id, data);
      }
    }
    void OnCANTransmit(nano_hw::can::CANMessage const& msg) {
      auto id = msg.id;
      std::vector<uint8_t> data(msg.data, msg.data + msg.len);

      for (auto& cb : tx_callbacks) {
        cb(id, data);
      }
    }
  };

  struct Handler {
    using OnCANReceived =
        nano_hw::Direct<[](void* instance,
                           nano_hw::can::CANMessage const& msg) {
          auto ctx = static_cast<Context*>(instance);
          ctx->OnCANReceived(msg);
        }>;
    using OnCANTransmit =
        nano_hw::Direct<[](void* instance,
                           nano_hw::can::CANMessage const& msg) {
          auto ctx = static_cast<Context*>(instance);
          ctx->OnCANTransmit(msg);
        }>;
    using OnCANBusError = nano_hw::Ignore;
    using OnCANPassiveError = nano_hw::Ignore;
  };
  static_assert(nano_hw::can::CANConfig<Handler>);

  Context ctx;
  std::vector<IdleCallback> idle_callbacks{};
  nano_hw::can::CANMessageFormat format_ =
      nano_hw::can::CANMessageFormat::kStandard;
  nano_hw::can::DynCAN<Handler> can_;
  nano_hw::thread::DynThread idle_caller_{ThreadPriorityNormal, 4096, nullptr,
                                          "SimpleCAN"};

 public:
  SimpleCAN(PinName rx, PinName tx, int frequency = 50E3,
            bool is_can_extended = false)
      : can_{nano_hw::Pin{static_cast<uint16_t>(tx)},
             nano_hw::Pin{static_cast<uint16_t>(rx)}, frequency, &ctx} {}
  virtual ~SimpleCAN() override = default;

  int Send(uint32_t id, std::vector<uint8_t> const& data) override {
    nano_hw::can::CANMessage msg;
    msg.id = id;
    msg.len = data.size();
    msg.format = format_;
    memcpy(msg.data, data.data(), data.size());
    return can_.SendMessage(msg) ? 1 : 0;
  }
  void SetCANExtended(bool is_can_extended) {
    format_ = is_can_extended ? nano_hw::can::CANMessageFormat::kExtended
                              : nano_hw::can::CANMessageFormat::kStandard;
  }

  void Init() override {
    idle_caller_.Start([this]() {
      while (true) {
        for (auto& cb : idle_callbacks) {
          cb();
        }

        nano_hw::parallel::SleepForMS(10ms);
      }
    });
  }

  void OnRx(RxCallback cb) override { ctx.rx_callbacks.push_back(cb); }
  void OnTx(TxCallback cb) override { ctx.tx_callbacks.push_back(cb); }
  void OnIdle(IdleCallback cb) override { idle_callbacks.push_back(cb); }
};
}  // namespace robotics::network