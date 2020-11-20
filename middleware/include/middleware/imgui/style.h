#pragma once

#include <cstdint>
#include <string_view>

#include "core/math/types.h"


namespace gui {

enum class Direction : uint8_t {
    Up,
    Down
};

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
    math::SizeF availableSize;
    VerticalAlign verticalAlign = VerticalAlign::Top;
    HorisontalAlign horisontalAlign = HorisontalAlign::Left;
    std::string_view tooltip;
};

void StartDisable();
void FinishDisable();

} // end namespace gui
