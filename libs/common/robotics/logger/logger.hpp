#pragma once

#include "logger-core.hpp"

#include "generic_logger.hpp"
#include "char_logger.hpp"

namespace robotics::logger {
using Logger = GenericLogger;
void SuppressLogger(const char* id);

void ResumeLogger(const char* id);

void Init();
}  // namespace robotics::logger
