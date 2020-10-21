#pragma once

#include "core/math/types.h"

namespace gui {

struct Style;

void ItemSize(math::SizeF widgetSize);
bool ItemAdd(uint32_t id, math::RectF widgetRect);
bool PlaceWidget(uint32_t id, math::SizeF widgetSize, math::RectF& widgetRect);
void PlaceWidgetCalc(const Style* style, math::SizeF drawSize, math::RectF& drawRect, math::RectF& widgetRect);
bool PlaceWidget(uint32_t id, const Style* style, math::SizeF drawSize, math::RectF& drawRect, math::RectF& widgetRect);

bool IsRectVisible(math::RectF rect);

} // end namespace gui
