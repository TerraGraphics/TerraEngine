#pragma once

#include "core/math/types.h"


namespace gui {

void SameLine();

math::RectF Dummy(math::SizeF size);

void BeginGroup();
math::RectF EndGroup();

} // end namespace gui
