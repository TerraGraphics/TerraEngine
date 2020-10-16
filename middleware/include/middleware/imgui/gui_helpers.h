#pragma once

#include <string_view>

#include "core/math/types.h"


namespace gui {

void ItemSize(math::SizeF size);
bool ItemAdd(math::RectF rect);
bool ItemFullAdd(math::RectF rect);

bool IsRectVisible(math::RectF rect);

} // end namespace gui
