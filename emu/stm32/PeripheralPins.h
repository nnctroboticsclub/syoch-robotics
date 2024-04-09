#pragma once

#include "PinName.h"

namespace mbed::peripherals {
typedef struct {
  PinName pin;
  int peripheral;
  int function;
} PinMap;

const PinMap PinMap_SPI_MOSI[] = {{NC, NC, 0}};

void *pinmap_peripheral(PinName pin, const PinMap *map) {}
}  // namespace mbed::peripherals

using namespace mbed::peripherals;