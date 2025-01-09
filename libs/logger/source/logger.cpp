#include <logger/logger.hpp>

namespace robotics::logger {

void SuppressLogger(const char* id) {
  auto logger = GenericLogger::GetLogger(id);
  if (logger == nullptr) {
    return;
  }
  logger->Supress();
}

void ResumeLogger(const char* id) {
  auto logger = GenericLogger::GetLogger(id);
  if (logger == nullptr) {
    return;
  }
  logger->Resume();
}
}  // namespace robotics::logger
