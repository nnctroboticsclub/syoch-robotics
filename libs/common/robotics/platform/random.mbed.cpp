#pragma once

#include <mbed.h>

#include "random.hpp"

#ifdef __MBED__
namespace {
mbed::AnalogIn source_{PC_0};
}

float robotics::system::Random::Entropy() { return source_.read(); }
#elif defined(ESP_PLATFORM)
#error "Not implemented"
#else
#error "Not implemented"
#endif