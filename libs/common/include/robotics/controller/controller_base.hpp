#pragma once

#include "../node/node.hpp"
#include "packet.hpp"

namespace controller {

struct GenericController {
 public:
  explicit GenericController(int id) : assigned_id_(id) {};
  virtual ~GenericController() = default;

  virtual bool Filter(RawPacket const& packet) = 0;
  virtual void Parse(RawPacket const& packet) = 0;

  bool Pass(RawPacket const& packet) {
    if (!this->Filter(packet)) {
      return false;
    }

    this->Parse(packet);

    return true;
  }

  int assigned_id_;
};
template <typename T>
struct ControllerBase : public GenericController, public robotics::Node<T> {
  using GenericController::GenericController;
};

}  // namespace controller
