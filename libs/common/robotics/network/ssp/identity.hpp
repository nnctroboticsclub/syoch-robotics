#pragma once

#include <cstring>

#include "kv.hpp"
#include "../stream.hpp"

namespace robotics::network::ssp {
template <typename Context = uint8_t>
class IdentitiyService : public robotics::network::ssp::KVService<Context> {
 public:
  IdentitiyService(robotics::network::Stream<uint8_t, uint8_t>& stream,
                   const char* device_name)
      : KVService<Context>(stream, 0x00, "id.svc.nw",
                           "\x1b[32mIdentitiyService\x1b[m") {
    using robotics::network::ssp::KVPacket;

    this->OnKVRequested(0x0000, [device_name]() {
      return (KVPacket){(uint8_t*)device_name, std::strlen(device_name)};
    });
    this->OnKVRequested(0x0001, []() {
      return (KVPacket){(uint8_t*)__DATE__, std::strlen(__DATE__)};
    });
    this->OnKVRequested(0x0002, []() {
      return (KVPacket){(uint8_t*)__TIME__, std::strlen(__TIME__)};
    });
  }
};
}  // namespace robotics::network::ssp