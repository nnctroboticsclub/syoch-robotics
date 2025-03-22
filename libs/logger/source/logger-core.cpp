#include <logger/logger-core.hpp>

#include <cstdint>
#include <cstdio>
#include <cstring>

#include <robotics/binary/temp_buffer.hpp>

#include <logger/log_sink.hpp>

namespace robotics::logger::core {

void inline Log(Level level, const char* tag, const char* msg, size_t) {
  global_log_sink->Log(level, tag, msg);
}

void Log(Level level, const char* tag, const char* msg) {
  Log(level, tag, msg, strlen(msg));
}

void LogHex(Level level, const char* tag, uint8_t* data, uint32_t length) {
  char* buffer =
      reinterpret_cast<char*>(robotics::binary::GetTemporaryBuffer());

  auto text_ptr = buffer;
  for (auto* data_ptr = data; data_ptr < data + length; data_ptr++) {
    *(text_ptr++) = "0123456789ABCDEF"[*data_ptr >> 4];
    *(text_ptr++) = "0123456789ABCDEF"[*data_ptr & 0x0F];
  }

  Log(level, tag, buffer, 2 * length);
}

void LoggerProcess() {}

void Init() {}

}  // namespace robotics::logger::core