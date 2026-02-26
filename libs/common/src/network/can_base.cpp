#include <robotics/network/can_base.hpp>

namespace robotics::network {

CANBase::Capability CANBase::GetCapability() { return Capability(); }

float CANBase::GetBusLoad() { return 0.0f; }

}  // namespace robotics::network