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
    math::RectOffsetF padding;
    HorisontalAlign horisontalAlign = HorisontalAlign::Left;
    VerticalAlign verticalAlign = VerticalAlign::Top;
};

} // end namespace gui
