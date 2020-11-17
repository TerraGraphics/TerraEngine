#pragma once

#include <cstdint>

#include "core/math/types.h"
#include "middleware/imgui/style.h"



struct ImDrawList;

namespace gui {

void RenderArrowIcon(ImDrawList* drawList, math::RectF rect, uint32_t color, Direction dir);

} // end namespace gui
