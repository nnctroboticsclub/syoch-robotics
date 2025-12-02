#pragma once

#include <cstdint>
#include <unordered_map>

#include <logger/logger.hpp>
#include <robotics/network/stream.hpp>

namespace robotics::network {
namespace ssp {
template <typename T, typename TxRet>
class SerialServiceProtocol;

template <typename Context = uint8_t, typename TxRet = void>
class SSP_Service : public Stream<uint8_t, Context, TxRet> {
  using StreamType = Stream<uint8_t, Context, TxRet>;
  StreamType& stream_;

  uint8_t service_id_;

 protected:
  logger::Logger logger;

 private:
  friend class SerialServiceProtocol<Context, TxRet>;

 public:
  SSP_Service(StreamType& stream, uint8_t service_id, const char* logger_tag,
              const char* logger_header)
      : stream_(stream),
        service_id_(service_id),
        logger(logger_tag, logger_header) {}

  TxRet Send(Context address, uint8_t* data, uint32_t length) override {
    static uint8_t buffer[128] = {};

    buffer[0] = service_id_;

    for (size_t i = 0; i < length; i++) {
      buffer[1 + i] = data[i];
    }

    return stream_.Send(address, buffer, length + 1);
  }

  uint8_t GetServiceId() const { return service_id_; }
};

template <typename Context, typename TxRet>
class SerialServiceProtocol {
  logger::Logger logger;
  Stream<uint8_t, Context, TxRet>& stream_;
  std::unordered_map<uint8_t, SSP_Service<Context, TxRet>*> services_;

 public:
  SerialServiceProtocol(Stream<uint8_t, Context, TxRet>& stream)
      : logger("ssp.nw.srobo1", "SerialServiceProtocol"), stream_(stream) {
    stream_.OnReceive(
        [this](Context from_addr, uint8_t* data, uint32_t length) {
          if (length < 1) {
            return;
          }

          uint8_t service_id = data[0];

          if (!services_.contains(service_id)) {
            logger.Error("Unknown service id: %d", service_id);
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