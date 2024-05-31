#include "logger.hpp"

namespace robotics::logger {

Logger system_logger{"system", "logger"};

void SuppressLogger(const char* id) {
  auto logger = GenericLogger::GetLogger(id);
  logger->Supress();
}

void ResumeLogger(const char* id) {
  auto logger = GenericLogger::GetLogger(id);
  logger->Resume();
}

void Init() {
  core::StartLoggerThread();
  system_logger.Info("Logger Initialized (from Log)");
}
}  // namespace robotics::logger
