#pragma once

#include "core/dg/dg.h"
#include "core/math/types.h"


enum class IconType : uint8_t {
    Flow,
    Circle,
    Square,
    Grid,
    RoundSquare,
    Diamond
};

math::Rect Image(TextureViewRaw texture, math::Size size, bool isOpenGL,
    math::PointF uv0 = math::PointF(0, 0), math::PointF uv1 = math::PointF(1, 1),
    math::Color tintCol = math::Color(255));


void NodeIcon(const math::Size& size, IconType type, bool filled,
    math::Color color = math::Color(255), math::Color innerColor = math::Color(0));