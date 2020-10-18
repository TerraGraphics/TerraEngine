#pragma once

#include <string_view>

#include "core/math/types.h"
#include "middleware/imgui/style.h"


namespace gui {

struct LabelStyle : Style {
};

void SameLine();
math::RectF Label(std::string_view text, const LabelStyle& style = LabelStyle(), math::SizeF minSize = math::SizeF());

} // end namespace gui
