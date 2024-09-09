#pragma once

#include <cstdint>
#include <cstring>
#include <unordered_map>

#include <robotics/node/node.hpp>
#include <robotics/utils/no_mutex_lifo.hpp>
#include <robotics/network/ssp/ssp.hpp>
#include <robotics/platform/thread.hpp>
#include <robotics/controller/controller_base.hpp>

namespace robotics::network::ssp {
class ValueStoreService : public robotics::network::ssp::SSP_Service {
  struct RxQueue {
    union {
      uint8_t raw_data[12];

      struct {
        uint32_t node_id;
        uint8_t data[8];
      };
    };
     size_t size;
  };

  std::unordered_map<uint32_t, robotics::node::GenericNode*> nodes_;

  robotics::utils::NoMutexLIFO<RxQueue, 4> rx_queue_;

  char rx_buffer[16];
  char tx_buffer[16];

 public:
  ValueStoreService(robotics::network::Stream<uint8_t, uint8_t>& stream)
      : SSP_Service(stream, 0x23, "vs.svc.nw",
                    "\x1b[33mValueStoreService\x1b[m") {
    OnReceive([this](uint8_t addr, uint8_t* data, size_t len) {
      if (len == 0) {
        return;
      }

      RxQueue rx;
      std::memcpy(rx.raw_data, data, len);
      rx.size = len;

      rx_queue_.Push(rx);
    });
  }

  void StartThread() {
    robotics::system::Thread thread;
    thread.SetStackSize(1024);
    thread.Start([this]() {
      while (1) {
        Process();
      }
    });
  }

  void Process() {
    if (this->rx_queue_.Empty()) {
      robotics::system::SleepFor(10ms);
      return;
    }

    auto rx = this->rx_queue_.Pop();
    auto node_id = rx.node_id;

    if (nodes_.find(node_id) == nodes_.end()) {
      return;
    }

    auto node = nodes_[node_id];
    node->Decode({rx.data[0], rx.data[1], rx.data[2], rx.data[3]});
  }

  void AddController(uint32_t id, uint8_t remote,
                     robotics::node::GenericNode& node) {
    node.OnChanged(
        [this, remote, node]() {
          auto data = node.Encode();
          this->Send(remote, data, 4);
        });
    nodes_.push_back(&node);
  }
};
}  // namespace robotics::network::ssp