#pragma once

#include "core/math/types.h"

namespace gui {

struct Style;

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

} // end namespace gui
