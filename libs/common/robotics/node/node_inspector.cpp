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

  std::array<uint8_t, 4> data_payload;

 public:
  uint16_t type;
  uint16_t id;
  std::vector<std::shared_ptr<NodeInfo>> links;

  NodeInfo(uint16_t type) : type(type), id(latest_id_++) {}

  void SendData();
  void SendLink(size_t index);
  void SendLinks();

  void SetData(std::array<uint8_t, 4> data) { data_payload = data; }
};

uint16_t NodeInfo::latest_id_ = 0;

class GlobalNodeInspector {
  std::shared_ptr<network::DistributedCAN> can_;
  uint32_t message_id;

  std::shared_ptr<network::Stream<uint8_t, uint8_t>> stream_;
  uint8_t stream_send_to;

  static inline std::vector<std::shared_ptr<NodeInfo>> nodes_;

  static void HandleCanMessage(std::uint32_t id, uint8_t *payload,
                               size_t size) {
    enum class Command : uint8_t {
      kShowAll,
      kRequestNodeValue,
      kRequestNodeLinks,
    };

    auto command = (Command)payload[0];

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
          if (node->id == id) {
            node->SendData();
            break;
          }
        }
        break;
      }

      case Command::kRequestNodeLinks: {
        uint16_t id = (payload[1] << 8) | payload[2];

        for (auto node : nodes_) {
          if (node->id == id) {
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
  void Send(uint16_t type, uint16_t id, std::array<uint8_t, 4> data) {
    uint8_t payload_bytes[8] = {
        (unsigned char)(type >> 8),
        (unsigned char)(type & 0xFF),
        (unsigned char)(id >> 8),
        (unsigned char)(id & 0xFF),
        data[0],
        data[1],
        data[2],
        data[3],
    };

    if (can_ != nullptr) {
      std::vector<uint8_t> payload = {payload_bytes, payload_bytes + 8};
      can_->Send(message_id, payload);
    }
    if (stream_ != nullptr) {
      stream_->Send(stream_send_to, payload_bytes, 8);
    }
  }

  void AddNode(std::shared_ptr<NodeInfo> node) { nodes_.push_back(node); }

  void RegisterCAN(std::shared_ptr<network::DistributedCAN> can) {
    can_ = can;
    message_id = 0x4d0 + can->GetDeviceId();

    can_->OnMessage(0x7f8, 0x4d8,
                    [this](uint32_t id, std::vector<uint8_t> payload) {
                      HandleCanMessage(id, payload.data(), payload.size());
                    });
  }

  void RegisterStream(
      std::shared_ptr<network::Stream<uint8_t, uint8_t>> stream) {
    stream_ = stream;

    stream->OnReceive([this](uint8_t from, uint8_t *data, size_t len) {
      this->stream_send_to = from;
      HandleCanMessage(from, data, len);
    });
  }
};

GlobalNodeInspector g_inspector;

void NodeInfo::SendData() { g_inspector.Send(type, id, data_payload); }

void NodeInfo::SendLink(size_t index) {
  if (0 < index || index >= links.size()) {
    return;
  }

  auto link = links[index];

  g_inspector.Send(
      0xFFFF, id,
      {(unsigned char)(link->type >> 8), (unsigned char)(link->type & 0xFF),
       (unsigned char)(link->id >> 8), (unsigned char)(link->id & 0xFF)});
}

void NodeInfo::SendLinks() {
  for (size_t i = 0; i < links.size(); i++) {
    SendLink(i);
  }
}
}  // namespace

class NodeInspector::Impl {
  NodeInfo node_info;

 public:
  Impl(uint16_t type) : node_info(type) {
    g_inspector.AddNode(std::shared_ptr<NodeInfo>(&node_info));
  }

  void Update(std::array<uint8_t, 4> data) {
    node_info.SetData(data);
    node_info.SendData();
  }

  void Link(std::shared_ptr<Impl> other) {
    auto info_p = std::shared_ptr<NodeInfo>(&other->node_info);

    node_info.links.push_back(info_p);

    node_info.SendLink(node_info.links.size() - 1);
  }

  static void RegisterCAN(std::shared_ptr<network::DistributedCAN> can) {
    g_inspector.RegisterCAN(can);
  }

  static void RegisterStream(
      std::shared_ptr<network::Stream<uint8_t, uint8_t>> stream) {
    g_inspector.RegisterStream(stream);
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

void NodeInspector::RegisterStream(
    std::shared_ptr<network::Stream<uint8_t, uint8_t>> stream) {
  Impl::RegisterStream(stream);
}

}  // namespace robotics::node