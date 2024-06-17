#pragma once

#include "packet.hpp"
#include "../node/node.hpp"

namespace controller {

struct GenericController {
 public:
  virtual bool Filter(RawPacket const &packet) = 0;
  virtual void Parse(RawPacket const &packet) = 0;

 public:
  int assigned_id_;

  GenericController(int id) : assigned_id_(id) {}

  bool Pass(RawPacket const &packet) {
    if (!this->Filter(packet)) {
      return false;
    }

    this->Parse(packet);

    return true;
  }
};
template <typename T>
struct ControllerBase : public GenericController, public robotics::Node<T> {
  using GenericController::GenericController;
};

}  // namespace controller
