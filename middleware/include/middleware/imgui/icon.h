#pragma once

#include "core/math/types.h"
#include "middleware/imgui/style.h"


namespace gui {

enum class IconType : uint8_t {
    Circle,
    CircleTriangle,
    Hexagon,
    HexagonTriangle,
    Tetragon,
    RoundSquare,
    RoundSquareTriangle,
    Square,
    SquareTriangle,
    Triangle,
    Flow
};

struct IconStyle : Style {
    math::Color color = math::Color(255);
    math::Color fillColor = math::Color(255);
    float sideSize = 24.f;
};

math::RectF Icon(IconType type, bool filled, const IconStyle& style = IconStyle(), math::SizeF minSize = math::SizeF());

} // end namespace gui
