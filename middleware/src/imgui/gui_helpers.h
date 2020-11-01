#pragma once

#include <cstdint>
#include "core/math/types.h"


struct ImGuiWindow;
namespace gui {

struct Style;

bool isOpenGL();

ImGuiWindow* GetCurrentWindow();
// fill outWidgetRect as empty if window is skipped
ImGuiWindow* GetCheckedCurrentWindow(math::RectF* outWidgetRect = nullptr);

void ItemSize(math::SizeF widgetSize);
bool ItemAdd(uint32_t id, math::RectF widgetRect);

// padding is empty
// margin is empty
// verticalAlign == Top;
// horisontalAlign == Left;
bool PlaceWidget(uint32_t id, math::SizeF widgetSize, math::RectF* outWidgetRect);

// outDrawRect::Size == drawSize
// outWidgetRect::Size >= Style::minSize
// outFullRect::Size >= outWidgetRect::Size >= outDrawRect::Size
void PlaceWidgetCalc(const Style* style, math::SizeF drawSize, math::RectF* outDrawRect, math::RectF* outWidgetRect, math::RectF* outFullRect);
bool PlaceWidget(uint32_t id, const Style* style, math::SizeF drawSize, math::RectF* outDrawRect, math::RectF* outWidgetRect);

bool IsRectVisible(math::RectF rect);

void DrawTooltip(const Style* style);

} // end namespace gui
