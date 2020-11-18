#pragma once

#include <cstdint>

#include "imgui/imgui.h"
#include "core/math/types.h"
#include "middleware/imgui/style.h"


namespace gui {

math::Color4 GetThemeColor(ImGuiCol_ idx);

void RenderArrowIcon(ImDrawList* drawList, math::RectF rect, math::Color4 color, Direction dir);

} // end namespace gui
