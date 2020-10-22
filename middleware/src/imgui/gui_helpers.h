#pragma once

#include "core/math/types.h"

namespace gui {

struct Style;

void ItemSize(math::SizeF widgetSize);
bool ItemAdd(math::RectF widgetRect);
bool PlaceWidget(math::SizeF widgetSize, math::RectF& widgetRect);
void PlaceWidgetCalc(const Style* style, math::SizeF minSize, math::SizeF drawSize, math::RectF& drawRect, math::RectF& widgetRect);
bool PlaceWidget(const Style* style, math::SizeF minSize, math::SizeF drawSize, math::RectF& drawRect, math::RectF& widgetRect);

bool IsRectVisible(math::RectF rect);

} // end namespace gui
