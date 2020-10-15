#pragma once

#include <string_view>

#include "core/math/types.h"


namespace gui {

enum class HorisontalAlign : uint8_t {
    Left,
    Center,
    Right
};

enum class VerticalAlign : uint8_t {
    Top,
    Center,
    Bottom
};

struct LabelStyle {
    math::RectOffsetF padding;
    HorisontalAlign horisontalAlign = HorisontalAlign::Left;
    VerticalAlign verticalAlign = VerticalAlign::Top;
};

void SameLine();
math::RectF Label(std::string_view text, const LabelStyle& style = LabelStyle(), math::SizeF minSize = math::SizeF());

} // end namespace gui
