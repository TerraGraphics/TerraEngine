#pragma once

#include "core/math/types.h"

struct ImVec2;
struct ImRect;

namespace gui {

ImVec2 ToImGui(math::PointU value);
ImVec2 ToImGui(math::PointI value);
ImVec2 ToImGui(math::PointF value);
ImVec2 ToImGui(math::SizeU value);
ImVec2 ToImGui(math::SizeF value);
ImRect ToImGui(math::RectF value);

math::PointU ToPointU(ImVec2 value);
math::PointF ToPointF(ImVec2 value);
math::SizeU ToSizeU(ImVec2 value);
math::SizeF ToSizeF(ImVec2 value);
math::RectF ToRectF(ImVec2 pointMin, ImVec2 pointMax);

}
