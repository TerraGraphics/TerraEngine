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
math::SizeF Label(std::string_view text, math::SizeF minSize, const LabelStyle& style);

} // end namespace gui
