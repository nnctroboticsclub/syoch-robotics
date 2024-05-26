#pragma once

#include <cstdint>

#include "stream.hpp"
#include "../logger/logger.hpp"

namespace robotics::network {
namespace ssp {
class SSP_Service : public Stream<uint8_t, uint8_t> {
  Stream<uint8_t, uint8_t>& stream_;

  uint16_t service_id_;

 private:
  friend class SerialServiceProtocol;

 public:
  SSP_Service(Stream<uint8_t, uint8_t>& stream, uint16_t service_id)
      : stream_(stream), service_id_(service_id) {}

  void Send(uint8_t address, uint8_t* data, uint32_t length) override {
    static uint8_t buffer[128] = {};

    buffer[0] = (service_id_ >> 8) & 0xFF;
    buffer[1] = service_id_ & 0xFF;
    buffer[2] = 0;
    buffer[3] = 0;

    for (size_t i = 0; i < length; i++) {
      buffer[4 + i] = data[i];
    }

    stream_.Send(address, buffer, length + 4);
  }

  uint16_t GetServiceId() const { return service_id_; }
};

class SerialServiceProtocol {
  static logger::Logger logger;

  Stream<uint8_t, uint8_t>& stream_;
  std::unordered_map<uint16_t, SSP_Service*> services_;

 public:
  SerialServiceProtocol(Stream<uint8_t, uint8_t>& stream) : stream_(stream) {
    stream_.OnReceive(
        [this](uint8_t from_addr, uint8_t* data, uint32_t length) {
          if (length < 4) {
            logger.Error("[SSP] Invalid Length: %d", length);
            return;
          }

          uint16_t service_id = (data[0] << 8) | data[1];

          if (services_.find(service_id) == services_.end()) {
            logger.Error("[SSP] Invalid Service: %d", service_id);
            return;
          }
          auto service = services_[service_id];

          service->DispatchOnReceive(from_addr, data + 4, length - 4);
        });
  }

  template <typename T>
  void RegisterService() {
    auto service = new T(stream_);
    services_[service->GetServiceId()] = service;
  }
};

logger::Logger SerialServiceProtocol::logger{"ssp.nw","\x1b[36mSSP\x1b[m"};
}  // namespace ssp
using ssp::SerialServiceProtocol;
using ssp::SSP_Service;
}  // namespace robotics::network