#pragma once

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#if defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wsign-promo"
#endif
#define IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DISABLE_OBSOLETE_FUNCTIONS
#include <imgui_internal.h>
#pragma GCC diagnostic pop
