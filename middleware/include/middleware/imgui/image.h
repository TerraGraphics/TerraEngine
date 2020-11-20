#pragma once

#include <cstdint>

#include "dg/dg.h"
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
    float sideSize = 24.f;
};

void Icon(IconType type, bool filled, const IconStyle& style = IconStyle(), math::RectF* outRect = nullptr);

struct ImageStyle : Style {
    math::Color color = math::Color(255);
    math::RectF uv = math::RectF(0, 0, 1.f, 1.f);
};

void Image(math::SizeF drawSize, const ImageStyle& style = ImageStyle(), math::RectF* outRect = nullptr);
void Image(math::SizeF drawSize, TextureViewPtr texture, const ImageStyle& style = ImageStyle(), math::RectF* outRect = nullptr);

} // end namespace gui
