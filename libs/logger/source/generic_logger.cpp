#include <logger/generic_logger.hpp>

#include <cstdint>
#include <cstdio>
#include <cstring>

#include <robotics/binary/temp_buffer.hpp>
#include <robotics/utils/no_mutex_lifo.hpp>

namespace robotics::logger {
using robotics::utils::NoMutexLIFO;
static GenericLogger* loggers[32] = {
    nullptr,
};

void GenericLogger::_Log(core::Level level, const char* fmt, va_list args) {
  if (supressed)
    return;

  char* line = reinterpret_cast<char*>(robotics::binary::GetTemporaryBuffer());

  vsnprintf(line, 0x80, fmt, args);

  core::Log(level, tag, line);
}

void GenericLogger::_LogHex(core::Level level, const uint8_t* buf,
                            size_t size) {
  if (supressed)
    return;
  if (size == 0)
    return;

  auto lines = size / 16;
  for (size_t line = 0; line <= lines; line++) {
    char* buffer =
        reinterpret_cast<char*>(robotics::binary::GetTemporaryBuffer());

    char* ptr = buffer;
    strncpy(ptr, "___| ", 6);

    for (int i = 0; i < 16 && line * 16 + i < size; i++) {
      auto ch = buf[line * 16 + i];
      *(ptr++) = "0123456789ABCDEF"[ch >> 4];
      *(ptr++) = "0123456789ABCDEF"[ch & 0x0F];

      if (i % 4 == 3) {
        *(ptr++) = ' ';
      }
    }

    *ptr = '\0';

    core::Log(level, tag, buffer);
  }
}

GenericLogger::GenericLogger(const char* id, const char* tag)
    : id(id), tag(tag) {
  for (int i = 0; i < 64; i++) {
    if (!loggers[i]) {
      loggers[i] = this;
      break;
    }
  }
}

void GenericLogger::RenameTag(const char* new_tag) {
  tag = new_tag;
}

void GenericLogger::Supress() {
  supressed = true;
}

void GenericLogger::Resume() {
  supressed = false;
}

void GenericLogger::Log(core::Level level, const char* fmt, ...) {
  va_list args;
  va_start(args, fmt);

  _Log(level, fmt, args);

  va_end(args);
}

void GenericLogger::Info(const char* fmt, ...) {
  va_list args;
  va_start(args, fmt);

  _Log(core::Level::kInfo, fmt, args);

  va_end(args);
}

void GenericLogger::Error(const char* fmt, ...) {
  va_list args;
  va_start(args, fmt);

  _Log(core::Level::kError, fmt, args);

  va_end(args);
}

void GenericLogger::Debug(const char* fmt, ...) {
  va_list args;
  va_start(args, fmt);

  _Log(core::Level::kDebug, fmt, args);

  va_end(args);
}

void GenericLogger::Trace(const char* fmt, ...) {
  va_list args;
  va_start(args, fmt);

  _Log(core::Level::kTrace, fmt, args);

  va_end(args);
}

void GenericLogger::Verbose(const char* fmt, ...) {
  va_list args;
  va_start(args, fmt);

  _Log(core::Level::kVerbose, fmt, args);

  va_end(args);
}

void GenericLogger::Hex(core::Level level, const uint8_t* data, size_t length) {
  if (supressed)
    return;
  _LogHex(level, data, length);
}

GenericLogger* GenericLogger::GetLogger(const char* id) {
  for (int i = 0; i < 64; i++) {
    if (loggers[i] && strcmp(loggers[i]->id, id) == 0) {
      return loggers[i];
    }
  }
  return nullptr;
}

}  // namespace robotics::logger
