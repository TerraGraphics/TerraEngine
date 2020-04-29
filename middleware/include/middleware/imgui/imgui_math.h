#pragma once

#include "core/math/types.h"

struct ImVec2;

namespace gui {

ImVec2 ToImGui(math::Size value);
ImVec2 ToImGui(math::Point value);
ImVec2 ToImGui(math::PointI value);
ImVec2 ToImGui(math::PointF value);
math::Point ToPoint(ImVec2 value);
math::PointF ToPointF(ImVec2 value);
math::Size ToSize(ImVec2 value);
math::RectF ToRectF(ImVec2 pointMin, ImVec2 pointMax);

}