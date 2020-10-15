#pragma once

#include "core/math/types.h"


namespace gui {

enum class IconType : uint8_t {
    Flow,
    Circle,
    Square,
    Grid,
    RoundSquare,
    Diamond
};

void Icon(math::SizeF size, IconType type, bool filled, math::Color color = math::Color(255), math::Color innerColor = math::Color(0));

} // end namespace gui
