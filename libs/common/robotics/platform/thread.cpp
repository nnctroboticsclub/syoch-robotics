#ifdef __MBED__
#include "thread.mbed.hpp"
#elif defined(ESP_PLATFORM)
#include "thread.idf.hpp"
#else
#include "thread.mock.hpp"
#endif