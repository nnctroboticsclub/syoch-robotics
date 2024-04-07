#pragma once

#include "packet.hpp"
#include "../robotics/node/node.hpp"

namespace controller {

template <typename T>
struct ControllerBase : public robotics::Node<T> {
 public:
  virtual bool Filter(RawPacket const &packet) = 0;
  virtual void Parse(RawPacket const &packet) = 0;

 public:
  int assigned_id_;

  ControllerBase(int id) : assigned_id_(id) {}

  bool Pass(RawPacket const &packet) {
    if (!this->Filter(packet)) {
      return false;
    }

    this->Parse(packet);

    return true;
  }
};

}  // namespace controller
