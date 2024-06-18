#pragma once

#include <cstdint>

#include "../network/dcan.hpp"
#include "../network/stream.hpp"

namespace robotics::node {

class NodeInspector {
  class Impl;

  std::shared_ptr<Impl> impl_;

 public:
  NodeInspector(uint16_t type);
  ~NodeInspector();

  void Link(NodeInspector &other_inspector);
  void Update(std::array<uint8_t, 4> data);

  static void RegisterCAN(
      std::shared_ptr<robotics::network::DistributedCAN> can);

  static void RegisterStream(
      std::shared_ptr<robotics::network::Stream<uint8_t, uint8_t>> stream);
};

}  // namespace robotics::node