#pragma once

#include <cstdint>

#include "core/math/types.h"


struct ImGuiWindow;
namespace gui {

struct Style;

ImGuiWindow* GetCurrentWindow();
// fill outWidgetRect as empty if window is skipped
ImGuiWindow* GetCheckedCurrentWindow(math::RectF* outWidgetRect = nullptr);

void ItemSize(math::SizeF widgetSize);
bool ItemAdd(uint32_t id, math::RectF widgetRect);

// return draw rect
math::RectF WidgetCalc(const Style* style, math::SizeF drawSize, math::RectF* outFullRect = nullptr);
bool WidgetPlace(uint32_t id, const Style* style, math::SizeF drawSize, math::RectF* outDrawRect, math::RectF* outFullRect);

bool IsRectVisible(math::RectF rect);

} // end namespace gui
