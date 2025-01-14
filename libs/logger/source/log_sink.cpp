#include <logger/log_sink.hpp>

#include <cstdio>
#include <cstring>

#include <robotics/binary/temp_buffer.hpp>

namespace robotics::logger {

void StdoutSink::Log(core::Level level, const char* tag, const char* msg) {
  using robotics::logger::core::Level;

  char level_header[14];
  // '\e'  '['  '1'  ';'  '3'  'X'  'm'  'X'
  // '\e'  '['  'm' '\0'

  switch (level) {
    case Level::kError:
      strncpy(level_header, "\x1b[1;31mE\x1b[m", 14);
      break;
    case Level::kInfo:
      strncpy(level_header, "\x1b[1;32mI\x1b[m", 14);
      break;
    case Level::kVerbose:
      strncpy(level_header, "\x1b[1;34mV\x1b[m", 14);
      break;
    case Level::kDebug:
      strncpy(level_header, "\x1b[1;36mD\x1b[m", 14);
      break;
    case Level::kTrace:
      strncpy(level_header, "\x1b[1;35mT\x1b[m", 14);
      break;

    default:
      strncpy(level_header, "\x1b[1;37m?\x1b[m", 14);
      break;
  }

  std::printf("%s [%s] %s\n", level_header, tag, msg);
}

LogSink* global_log_sink = new StdoutSink();

}  // namespace robotics::logger