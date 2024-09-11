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
template <typename Context = uint8_t>
class ValueStoreService : public robotics::network::ssp::SSP_Service<Context> {
  union RxItem {
    uint8_t raw_data[8];
    struct {
      uint32_t id;
      uint8_t data[4];
    };
  };

  std::unordered_map<uint32_t, robotics::node::GenericNode*> nodes_;

  uint8_t tx_buffer[16];

 public:
  ValueStoreService(robotics::network::Stream<uint8_t, Context>& stream)
      : SSP_Service<Context>(stream, 0x23, "vs.svc.nw",
                             "\x1b[33mValueStoreService\x1b[m") {
    this->OnReceive([this](Context addr, uint8_t* data, size_t len) {
      if (len == 0) {
        return;
      }

      RxItem rx;
      std::memcpy(&rx.raw_data, data, len);

      if (nodes_.find(rx.id) == nodes_.end()) {
        return;
      }

      auto node = nodes_[rx.id];
      node->LoadFromBytes({rx.data[0], rx.data[1], rx.data[2], rx.data[3]});
    });
  }

  void AddController(uint32_t id, Context remote,
                     robotics::node::GenericNode& node) {
    node.OnChanged([this, id, remote, &node]() {
      auto data = node.Encode();
      tx_buffer[0] = (id >> 24) & 0xff;
      tx_buffer[1] = (id >> 16) & 0xff;
      tx_buffer[2] = (id >> 8) & 0xff;
      tx_buffer[3] = id & 0xff;
      std::memcpy(&tx_buffer[4], &data.front(), 4);

      this->Send(remote, tx_buffer, 8);
    });
    nodes_[id] = &node;
  }
};
}  // namespace robotics::network::ssp