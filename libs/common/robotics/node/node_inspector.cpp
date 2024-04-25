#include "node_inspector.hpp"

namespace robotics::node {
std::uint16_t NodeInspector::latest_id_ = 0;
std::shared_ptr<network::DistributedCAN> NodeInspector::can = nullptr;
}