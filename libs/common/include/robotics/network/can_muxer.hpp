#pragma once

#include <utility>

#include <logger/logger.hpp>
#include <robotics/network/can_base.hpp>

namespace robotics::network {
/// @brief 2 つの CAN を 1 つの CAN として扱うやつ
class CANMix : public robotics::network::CANBase {
  // static inline robotics::logger::Logger logger{"mixer.can.nw", "CANMix"};

 public:
  enum class CANSelection {
    kCAN0,
    kCAN1,
  };

 public:
  CANMix(robotics::network::CANBase* can1, robotics::network::CANBase* can2)
      : can1(std::move(can1)), can2(std::move(can2)) {
    can1->OnRx([this](uint32_t id, std::vector<uint8_t> const& data) {
      for (auto& cb : rx_cbs) {
        cb(id, data);
      }
      return;
    });
    can2->OnRx([this](uint32_t id, std::vector<uint8_t> const& data) {
      for (auto& cb : rx_cbs) {
        cb(id, data);
      }
      return;
    });
  }

  void Init() override {
    // No Initialization is needed
  }
  auto Send(uint32_t id, std::vector<uint8_t> const& data) -> int override {
    switch (selection) {
      case CANSelection::kCAN0:
        return can1->Send(id, data);
      case CANSelection::kCAN1:
        return can2->Send(id, data);
      default:
        return -1;
    }
  }

  void OnRx(RxCallback cb) override { rx_cbs.push_back(cb); }

  void OnTx(TxCallback) override {
    // logger.Error("OnTx is not implemented");
    // Not implemented
  }

  void OnIdle(IdleCallback) override {
    // logger.Error("OnIdle is not implemented");
    // Not implemented
  }

  void Select(CANSelection selection) { this->selection = selection; }

 private:
  robotics::network::CANBase* can1;
  robotics::network::CANBase* can2;
  CANSelection selection = CANSelection::kCAN0;

  std::vector<RxCallback> rx_cbs;
};
}  // namespace robotics::network