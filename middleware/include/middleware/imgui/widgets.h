#pragma once

#include "core/dg/dg.h"
#include "core/math/basic.h"


math::Rect Image(TextureViewRaw texture, math::Size size, bool isOpenGL,
    math::Pointf uv0 = math::Pointf(0, 0), math::Pointf uv1 = math::Pointf(1, 1),
    math::Color tintCol = math::Color(255));
