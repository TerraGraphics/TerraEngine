#pragma once

#include <cstdint>
#include <sys/types.h>

#include "platforms/window_events.h"


Key KeySymToKey(uint32_t value);
uint8_t StateToModifiers(uint state);
