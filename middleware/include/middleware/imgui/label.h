#pragma once

#include <string_view>

#include "core/math/types.h"
#include "middleware/imgui/style.h"


namespace gui {

struct LabelStyle : Style {
};

math::RectF LabelCalc(std::string_view text, const LabelStyle& style = LabelStyle());
void Label(std::string_view text, const LabelStyle& style = LabelStyle(), math::RectF* outWidgetRect = nullptr);

} // end namespace gui
