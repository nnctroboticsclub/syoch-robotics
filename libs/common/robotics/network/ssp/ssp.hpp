#pragma once

#include <unordered_map>
#include <cstdint>

#include "../stream.hpp"
#include "../../logger/logger.hpp"

namespace robotics::network {
namespace ssp {
class SSP_Service : public Stream<uint8_t, uint8_t> {
  Stream<uint8_t, uint8_t>& stream_;

  uint16_t service_id_;

 private:
  friend class SerialServiceProtocol;

 public:
  SSP_Service(Stream<uint8_t, uint8_t>& stream, uint16_t service_id);

  void Send(uint8_t address, uint8_t* data, uint32_t length) override;

  uint16_t GetServiceId() const;
};

class SerialServiceProtocol {
  Stream<uint8_t, uint8_t>& stream_;
  std::unordered_map<uint16_t, SSP_Service*> services_;

 public:
  SerialServiceProtocol(Stream<uint8_t, uint8_t>& stream);

  template <typename T>
  void RegisterService() {
    auto service = new T(stream_);
    services_[service->GetServiceId()] = service;
  }
};

}  // namespace ssp
using ssp::SerialServiceProtocol;
using ssp::SSP_Service;
}  // namespace robotics::network