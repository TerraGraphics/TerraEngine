#pragma once

#include "core/math/types.h"


namespace gui {

void SameLine();

math::RectF Dummy(math::SizeF size);

void BeginHorizontal();
math::RectF EndHorizontal();

void BeginVertical();
math::RectF EndVertical();

} // end namespace gui
