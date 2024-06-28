#pragma once

#include <unordered_map>
#include <cstdint>

#include "../stream.hpp"
#include "../../logger/logger.hpp"

namespace robotics::network {
namespace ssp {
class SSP_Service : public Stream<uint8_t, uint8_t> {
  Stream<uint8_t, uint8_t>& stream_;

  uint8_t service_id_;

 protected:
  logger::Logger logger;

 private:
  friend class SerialServiceProtocol;

 public:
  SSP_Service(Stream<uint8_t, uint8_t>& stream, uint8_t service_id,
              const char* logger_tag, const char* logger_header);

  void Send(uint8_t address, uint8_t* data, uint32_t length) override;

  uint8_t GetServiceId() const;
};

class SerialServiceProtocol {
  Stream<uint8_t, uint8_t>& stream_;
  std::unordered_map<uint8_t, SSP_Service*> services_;

 public:
  SerialServiceProtocol(Stream<uint8_t, uint8_t>& stream);

  template <typename T, typename... Args>
  T* RegisterService(Args... args) {
    auto service = new T(stream_, args...);
    services_[service->GetServiceId()] = service;

    return service;
  }
};

}  // namespace ssp
using ssp::SerialServiceProtocol;
using ssp::SSP_Service;
}  // namespace robotics::network