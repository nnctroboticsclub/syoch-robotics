#pragma once

#include <cstdint>
#include <cstring>

#include <robotics/utils/no_mutex_lifo.hpp>
#include <robotics/network/ssp/ssp.hpp>
#include <robotics/platform/thread.hpp>
#include <robotics/controller/controller_base.hpp>

namespace robotics::network::ssp {
class ValueStoreService : public robotics::network::ssp::SSP_Service {
  struct RxQueue {
    uint8_t data[12];
    size_t size;
  };

  std::vector<controller::GenericController*> controllers_;

  robotics::utils::NoMutexLIFO<RxQueue, 4> rx_queue_;

  void Thread() {
    while (1) {
      if (this->rx_queue_.Empty()) {
        robotics::system::SleepFor(10ms);
        continue;
      }

      auto rx = this->rx_queue_.Pop();
      auto raw_packet = controller::RawPacket(rx.data, rx.size);

      for (auto controller : controllers_) {
        controller->Pass(raw_packet);
      }
    }
  }

 public:
  ValueStoreService(robotics::network::Stream<uint8_t, uint8_t>& stream)
      : SSP_Service(stream, 0x10, "vs.svc.nw",
                    "\x1b[33mValueStoreService\x1b[m") {
    OnReceive([this](uint8_t addr, uint8_t* data, size_t len) {
      if (len == 0) {
        return;
      }

      RxQueue rx;
      std::memcpy(rx.data, data, len);
      rx.size = len;

      rx_queue_.Push(rx);
    });

    robotics::system::Thread thread;
    thread.SetStackSize(1024);
    thread.Start([this]() { this->Thread(); });
  }

  void AddController(controller::GenericController& controller) {
    controllers_.push_back(&controller);
  }
};
}  // namespace robotics::network::ssp