#pragma once

#include "controller_base.hpp"

namespace controller {

struct Action : public ControllerBase<bool> {
 private:
  std::vector<std::function<void()>> handlers;

 public:
  using ControllerBase::ControllerBase;

  bool Filter(RawPacket const& packet) override {
    return packet.element_id == (assigned_id_ | 0x80);
  }

  void Parse(RawPacket const& packet) override {
    if (packet.element_id & 0x80)
      for (auto const& handler : handlers) {
        handler();
      }
  }

  void operator>>(std::function<void()> const& handler) {
    handlers.push_back(handler);
  }
};

}  // namespace controller