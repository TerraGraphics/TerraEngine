#pragma once

#include <cstdint>

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

struct Style {
    math::RectOffsetF margin;
    math::RectOffsetF padding;
    math::SizeF minWidgetSize;
    VerticalAlign verticalAlign = VerticalAlign::Top;
    HorisontalAlign horisontalAlign = HorisontalAlign::Left;
    std::string_view tooltip;
};

} // end namespace gui
