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
    HorisontalAlign horisontalAlign = HorisontalAlign::Left;
    VerticalAlign verticalAlign = VerticalAlign::Top;
    math::RectOffsetF padding = DEFUALT_PADDING;

    constexpr static const math::RectOffsetF DEFUALT_PADDING = math::RectOffsetF(8.f, 0, 0, 0);
};

} // end namespace gui
