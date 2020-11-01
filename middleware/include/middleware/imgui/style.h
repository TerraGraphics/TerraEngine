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
    math::RectOffsetF margin = DEFUALT_MARGIN;
    math::RectOffsetF padding = DEFUALT_PADDING;
    math::SizeF minSize;
    VerticalAlign verticalAlign = VerticalAlign::Top;
    HorisontalAlign horisontalAlign = HorisontalAlign::Left;
    std::string_view tooltip;

    constexpr static const math::RectOffsetF DEFUALT_MARGIN = math::RectOffsetF(8.f, 0, 0, 0);
    constexpr static const math::RectOffsetF DEFUALT_PADDING = math::RectOffsetF();
};

} // end namespace gui
