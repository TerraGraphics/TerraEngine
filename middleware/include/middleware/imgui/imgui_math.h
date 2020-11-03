#pragma once

#include "core/math/types.h"

struct ImVec2;
struct ImVec4;
struct ImRect;

namespace gui {

ImVec2 ToImGui(math::PointU value);
ImVec2 ToImGui(math::PointI value);
ImVec2 ToImGui(math::PointF value);
ImVec2 ToImGui(math::SizeU value);
ImVec2 ToImGui(math::SizeF value);
ImRect ToImGuiRect(math::RectF value);
ImVec4 ToImGui(math::RectF value);
ImVec4 ToImGui(math::RectOffsetF value);
ImVec4 ToImGui(math::Color4f value);

math::PointU ToPointU(ImVec2 value);
math::PointF ToPointF(ImVec2 value);
math::SizeU ToSizeU(ImVec2 value);
math::SizeF ToSizeF(ImVec2 value);
math::RectF ToRectF(ImRect value);
math::RectF ToRectF(ImVec4 value);
math::RectF ToRectF(ImVec2 pointMin, ImVec2 pointMax);

}
