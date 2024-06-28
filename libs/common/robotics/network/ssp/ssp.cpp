#include "ssp.hpp"

namespace {
robotics::logger::Logger ssp_logger{"ssp.nw", "\x1b[36mSSP\x1b[m"};
}

namespace robotics::network::ssp {
SSP_Service::SSP_Service(Stream<uint8_t, uint8_t>& stream, uint8_t service_id,
                         const char* logger_tag, const char* logger_header)
    : logger(logger_tag, logger_header),
      stream_(stream),
      service_id_(service_id) {}

void SSP_Service::Send(uint8_t address, uint8_t* data, uint32_t length) {
  static uint8_t buffer[128] = {};

  buffer[0] = service_id_;

  for (size_t i = 0; i < length; i++) {
    buffer[1 + i] = data[i];
  }

  stream_.Send(address, buffer, length + 1);
}

uint8_t SSP_Service::GetServiceId() const { return service_id_; }

SerialServiceProtocol::SerialServiceProtocol(Stream<uint8_t, uint8_t>& stream)
    : stream_(stream) {
  stream_.OnReceive([this](uint8_t from_addr, uint8_t* data, uint32_t length) {
    ssp_logger.Hex(robotics::logger::core::Level::kDebug, data, length);
    if (length < 1) {
      ssp_logger.Error("[SSP] Invalid Length: %d", length);
      return;
    }

    uint8_t service_id = data[0] << 8;

    if (services_.find(service_id) == services_.end()) {
      ssp_logger.Error("[SSP] Unknown Service: %d", service_id);
      return;
    }
    auto service = services_[service_id];

    service->DispatchOnReceive(from_addr, data + 1, length - 1);
  });
}

}  // namespace robotics::network::ssp
