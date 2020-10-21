#pragma once

#include <string_view>

#include "core/math/types.h"
#include "middleware/imgui/style.h"


namespace gui {

void ItemSize(math::SizeF widgetSize);
bool ItemAdd(math::RectF widgetRect);
bool PlaceWidget(math::SizeF widgetSize, math::RectF& widgetRect);
void PlaceWidgetCalc(const Style* style, math::SizeF minSize, math::SizeF drawSize, math::RectF& drawRect, math::RectF& widgetRect);
bool PlaceWidget(const Style* style, math::SizeF minSize, math::SizeF drawSize, math::RectF& drawRect, math::RectF& widgetRect);

bool IsRectVisible(math::RectF rect);

} // end namespace gui
