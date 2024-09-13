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
template <typename Context = uint8_t, typename TxRet = void>
class ValueStoreService
    : public robotics::network::ssp::SSP_Service<Context, TxRet> {
  union RxItem {
    uint8_t raw_data[8];
    struct {
      uint32_t id;
      uint8_t data[4];
    };
  };

  struct RegisteredNode {
    robotics::node::GenericNode* node;
    bool receiving;
  };

  std::unordered_map<uint32_t, RegisteredNode> nodes_;

  std::function<void(Context, uint32_t)> on_node_received_;

  uint8_t tx_buffer[16];

 public:
  ValueStoreService(robotics::network::Stream<uint8_t, Context, TxRet>& stream)
      : SSP_Service<Context, TxRet>(stream, 0x23, "vs.svc.nw",
                                    "\x1b[33mValueStoreService\x1b[m"),
        on_node_received_([](Context, uint32_t){}) {
    this->OnReceive([this](Context addr, uint8_t* data, size_t len) {
      if (len == 0) {
        return;
      }

      RxItem rx;
      rx.id = (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3];
      std::memcpy(&rx.data[0], &data[4], 4);

      if (nodes_.find(rx.id) == nodes_.end()) {
        this->logger.Error("[RX] Node not found (node = %08x)", rx.id);
        return;
      }

      if (on_node_received_) {
        on_node_received_(addr, rx.id);
      }

      auto& node = nodes_[rx.id];
      node.receiving = true;
      node.node->LoadFromBytes(
          {rx.data[0], rx.data[1], rx.data[2], rx.data[3]});
      node.receiving = false;
    });
  }

  void OnNodeReceived(std::function<void(Context, uint32_t)> callback) {
    on_node_received_ = callback;
  }

  void AddController(uint32_t id, Context remote,
                     robotics::node::GenericNode& node) {
    node.OnChanged([this, id, remote, &node]() {
      if (nodes_.find(id) == nodes_.end()) {
        this->logger.Verbose("[Tx] Node not found (0x%08x)", id);
        return;
      }

      auto data = node.Encode();
      tx_buffer[0] = (id >> 24) & 0xff;
      tx_buffer[1] = (id >> 16) & 0xff;
      tx_buffer[2] = (id >> 8) & 0xff;
      tx_buffer[3] = id & 0xff;
      std::memcpy(&tx_buffer[4], &data.front(), 4);

      if (nodes_[id].receiving) {
        return;
      }

      this->Send(remote, tx_buffer, 8);
    });
    nodes_[id] = RegisteredNode{&node, false};
  }
};
}  // namespace robotics::network::ssp