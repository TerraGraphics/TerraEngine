#include "middleware/imgui/imgui_math.h"

#include <imgui.h>
#include <cstdint>


ImVec2 ToImGui(math::Size value) {
    return ImVec2(static_cast<float>(value.w), static_cast<float>(value.h));
}

ImVec2 ToImGui(math::Point value) {
    return ImVec2(value.x, value.y);
}

ImVec2 ToImGui(math::PointF value) {
    return ImVec2(value.x, value.y);
}

math::Point ToPoint(ImVec2 value) {
    return math::Point(static_cast<uint32_t>(value.x), static_cast<uint32_t>(value.y));
}

math::PointF ToPointF(ImVec2 value) {
    return math::PointF(value.x, value.y);
}

math::Size ToSize(ImVec2 value) {
    return math::Size(static_cast<uint32_t>(value.x), static_cast<uint32_t>(value.y));
}