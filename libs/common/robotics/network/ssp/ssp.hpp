#pragma once

#include <unordered_map>
#include <cstdint>

#include "../stream.hpp"
#include "../../logger/logger.hpp"

namespace robotics::network {
namespace ssp {
template <typename T>
class SerialServiceProtocol;

template <typename Context = uint8_t>
class SSP_Service : public Stream<uint8_t, Context> {
  Stream<uint8_t, Context>& stream_;

  uint8_t service_id_;

 protected:
  logger::Logger logger;

 private:
  friend class SerialServiceProtocol<Context>;

 public:
  SSP_Service(Stream<uint8_t, Context>& stream, uint8_t service_id,
              const char* logger_tag, const char* logger_header)
      : logger(logger_tag, logger_header),
        stream_(stream),
        service_id_(service_id) {}

  void Send(Context address, uint8_t* data, uint32_t length) override {
    static uint8_t buffer[128] = {};

    buffer[0] = service_id_;

    for (size_t i = 0; i < length; i++) {
      buffer[1 + i] = data[i];
    }

    stream_.Send(address, buffer, length + 1);
  }

  uint8_t GetServiceId() const { return service_id_; }
};

template <typename Context>
class SerialServiceProtocol {
  Stream<uint8_t, Context>& stream_;
  std::unordered_map<uint8_t, SSP_Service<Context>*> services_;

 public:
  SerialServiceProtocol(Stream<uint8_t, Context>& stream) : stream_(stream) {
    stream_.OnReceive(
        [this](Context from_addr, uint8_t* data, uint32_t length) {
          if (length < 1) {
            return;
          }

          uint8_t service_id = data[0];

          if (services_.find(service_id) == services_.end()) {
            printf("Service not found: %02x\n", service_id);
            return;
          }
          auto service = services_[service_id];

          service->DispatchOnReceive(from_addr, data + 1, length - 1);
        });
  }

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