#include "node_inspector.hpp"

#include <cstdint>

#include <array>
#include <vector>
#include <memory>

#include "../network/dcan.hpp"

namespace robotics::node {

namespace {
class NodeInfo {
  static uint16_t latest_id_;

 public:
  uint16_t type;
  uint16_t id;

  std::vector<std::shared_ptr<NodeInfo>> links;

  NodeInfo(uint16_t type) : type(type), id(latest_id_++) {}
};

uint16_t NodeInfo::latest_id_ = 0;
}  // namespace

class NodeInspector::Impl {
  NodeInfo node_info;

  std::array<uint8_t, 4> data_payload;

  void Send(uint16_t type, std::array<uint8_t, 4> data) {
    if (can_.can_ == nullptr) {
      return;
    }

    std::vector<uint8_t> payload{
        (unsigned char)(type >> 8),
        (unsigned char)(type & 0xFF),
        (unsigned char)(node_info.id >> 8),
        (unsigned char)(node_info.id & 0xFF),
        data[0],
        data[1],
        data[2],
        data[3],
    };

    can_.can_->Send(can_.message_id, payload);
  }

  void SendLink(int index) {
    if (index < 0 || index >= node_info.links.size()) {
      return;
    }

    Send(0xFFFF, {(unsigned char)(node_info.links[index]->type >> 8),
                  (unsigned char)(node_info.links[index]->type & 0xFF),
                  (unsigned char)(node_info.links[index]->id >> 8),
                  (unsigned char)(node_info.links[index]->id & 0xFF)});
  }

  void SendData() { Send(node_info.type, data_payload); }

  void SendLinks() {
    if (can_.can_ == nullptr) {
      return;
    }

    for (int i = 0; i < node_info.links.size(); i++) {
      SendLink(i);
    }
  }

  // CAN Integration
  static inline struct CAN {
    std::shared_ptr<network::DistributedCAN> can_;
    uint32_t message_id;
  } can_;
  static inline std::vector<std::shared_ptr<NodeInspector::Impl>> nodes_;

  static void HandleCanMessage(std::uint32_t id, std::vector<uint8_t> payload) {
    enum class Command : uint8_t {
      kShowAll,
      kRequestNodeValue,
      kRequestNodeLinks,
    };

    Command command = (Command)payload[0];

    switch (command) {
      case Command::kShowAll: {
        for (auto node : nodes_) {
          node->SendData();
          node->SendLinks();
        }
        break;
      }

      case Command::kRequestNodeValue: {
        uint16_t id = (payload[1] << 8) | payload[2];

        for (auto node : nodes_) {
          if (node->node_info.id == id) {
            node->SendData();
            break;
          }
        }
        break;
      }

      case Command::kRequestNodeLinks: {
        uint16_t id = (payload[1] << 8) | payload[2];

        for (auto node : nodes_) {
          if (node->node_info.id == id) {
            node->SendLinks();
            break;
          }
        }
        break;
      }

      default: {
        printf("Unknown command: %d\n", (int)command);
        break;
      }
    }
  }

 public:
  Impl(uint16_t type) : node_info(type) {
    nodes_.push_back((std::shared_ptr<NodeInspector::Impl>)this);
  }

  void Update(std::array<uint8_t, 4> data) {
    data_payload = data;
    SendData();
  }

  void Link(std::shared_ptr<Impl> other) {
    auto info_p = std::shared_ptr<NodeInfo>(&other->node_info);

    node_info.links.push_back(info_p);

    SendLink(node_info.links.size() - 1);
  }

  static void RegisterCAN(std::shared_ptr<network::DistributedCAN> can) {
    can_.can_ = can;
    can_.message_id = 0x4d0 + can->GetDeviceId();

    can_.can_->OnMessage(0x7f8, 0x4d8, HandleCanMessage);
  }
};

NodeInspector::NodeInspector(uint16_t type)
    : impl_(std::make_shared<Impl>(type)) {}

NodeInspector::~NodeInspector() = default;

void NodeInspector::Link(NodeInspector &other_inspector) {
  impl_->Link(other_inspector.impl_);
}

void NodeInspector::Update(std::array<uint8_t, 4> data) { impl_->Update(data); }

void NodeInspector::RegisterCAN(std::shared_ptr<network::DistributedCAN> can) {
  Impl::RegisterCAN(can);
}

}  // namespace robotics::node