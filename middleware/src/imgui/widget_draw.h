#pragma once

#include <cstdint>

#include "imgui/imgui.h"
#include "core/math/types.h"
#include "middleware/imgui/style.h"


namespace gui {

bool isOpenGL();
float GetDefaultFieldHeight();
math::SizeF GetDefaultButtonSize();
float GetStepButtonsWidth(float height);
math::Color4 GetThemeColor(ImGuiCol_ idx);

void DrawTooltip(const Style* style);
void DrawArrowIcon(ImDrawList* drawList, math::RectF rect, math::Color4 color, Direction dir);

} // end namespace gui
