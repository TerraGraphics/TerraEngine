#pragma once

#include <imgui.h>
#include "core/math/basic.h"


ImVec2 ToImGui(math::Size value);
ImVec2 ToImGui(math::Point value);
ImVec2 ToImGui(math::Pointf value);
math::Point ToPoint(ImVec2 value);
math::Pointf ToPointf(ImVec2 value);
math::Size ToSize(ImVec2 value);
