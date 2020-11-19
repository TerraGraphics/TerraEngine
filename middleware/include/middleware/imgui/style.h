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
    math::SizeF availableSize = math::SizeF(MINIMUM_REQUIRED);
    VerticalAlign verticalAlign = VerticalAlign::Top;
    HorisontalAlign horisontalAlign = HorisontalAlign::Left;
    std::string_view tooltip;

    static constexpr float ALL_AVAILABLE = std::numeric_limits<float>::max();
    static constexpr float MINIMUM_REQUIRED = std::numeric_limits<float>::quiet_NaN();
};

void StartDisable();
void FinishDisable();

} // end namespace gui
