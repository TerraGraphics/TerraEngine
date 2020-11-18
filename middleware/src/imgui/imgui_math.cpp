#include "middleware/imgui/imgui_math.h"

#include <cstdint>

#include "imgui/imgui.h"
#include "imgui/internal.h"


namespace gui {

ImVec2 ToImGui(math::PointU value) {
    return ImVec2(static_cast<float>(value.x), static_cast<float>(value.y));
}

ImVec2 ToImGui(math::PointI value) {
    return ImVec2(static_cast<float>(value.x), static_cast<float>(value.y));
}

ImVec2 ToImGui(math::PointF value) {
    return ImVec2(value.x, value.y);
}

ImVec2 ToImGui(math::SizeU value) {
    return ImVec2(static_cast<float>(value.w), static_cast<float>(value.h));
}

ImVec2 ToImGui(math::SizeF value) {
    return ImVec2(value.w, value.h);
}

ImVec4 ToImGui(math::RectF value) {
    return ImVec4(value.x, value.y, value.x + value.w, value.y + value.h);
}

ImRect ToImGuiRect(math::RectF value) {
    return ImRect(value.x, value.y, value.x + value.w, value.y + value.h);
}

ImVec4 ToImGui(math::RectOffsetF value) {
    return ImVec4(value.left, value.top, value.right, value.bottom);
}

ImVec4 ToImGui(math::Color4f value) {
    return ImVec4(value.red, value.green, value.blue, value.alpha);
}

math::PointU ToPointU(ImVec2 value) {
    return math::PointU(static_cast<uint32_t>(value.x), static_cast<uint32_t>(value.y));
}

math::PointF ToPointF(ImVec2 value) {
    return math::PointF(value.x, value.y);
}

math::SizeU ToSizeU(ImVec2 value) {
    return math::SizeU(static_cast<uint32_t>(value.x), static_cast<uint32_t>(value.y));
}

math::SizeF ToSizeF(ImVec2 value) {
    return math::SizeF(value.x, value.y);
}

math::RectF ToRectF(ImRect value) {
    return math::RectF(value.Min.x, value.Min.y, value.GetWidth(), value.GetHeight());
}

math::RectF ToRectF(ImVec4 value) {
    return math::RectF(value.x, value.y, value.z - value.x, value.w - value.y);
}

math::RectF ToRectF(ImVec2 pointMin, ImVec2 pointMax) {
    return math::RectF(ToPointF(pointMin), ToPointF(pointMax));
}

math::Color4 ToColor4(ImVec4 value) {
    return math::Color4(value.x, value.y, value.z, value.w);
}

} // namespace gui
