#pragma once

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#if defined(__clang__)
#pragma clang diagnostic ignored "-Wimplicit-int-float-conversion"
#elif defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wconversion"
#endif
#include <FastNoiseLite.h>
#pragma GCC diagnostic pop
