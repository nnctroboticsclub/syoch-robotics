#include "logger.hpp"

namespace robotics::logger {

Logger system_logger{"system", "logger"};

void SuppressLogger(const char* id) {
  auto logger = GenericLogger::GetLogger(id);
  if (logger == nullptr) {
    system_logger.Error("Logger %s not found", id);
    return;
  }
  logger->Supress();
}

void ResumeLogger(const char* id) {
  auto logger = GenericLogger::GetLogger(id);
  if (logger == nullptr) {
    system_logger.Error("Logger %s not found", id);
    return;
  }
  logger->Resume();
}

void Init() { core::StartLoggerThread(); }
}  // namespace robotics::logger
