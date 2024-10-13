#pragma once

#include "dout.interface.hpp"
#ifdef __MBED__
#include "dout.mbed.hpp"
#elif defined(ESP_PLATFORM)
#include "dout.idf.hpp"
#endif