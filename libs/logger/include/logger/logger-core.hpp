#pragma once

#include "./level.hpp"

namespace robotics::logger::details {

void LoggerProcess();
void StartLoggerThread_Impl();

template <typename T>
void StartLoggerThread(T _) {
  StartLoggerThread_Impl();
}
}  // namespace robotics::logger::details

namespace robotics::logger::core {
using details::LoggerProcess;

void Log(Level level, const char* tag, const char* msg);
void LogHex(Level level, const char* tag, uint8_t data, uint32_t len);

void Init();

inline void StartLoggerThread() {
  details::StartLoggerThread(0);
}

}  // namespace robotics::logger::core