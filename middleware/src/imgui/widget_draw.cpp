#include "imgui/widget_draw.h"

#include "imgui/imgui.h"
#include "middleware/imgui/imgui_math.h"


namespace gui {

void RenderArrowIcon(ImDrawList* drawList, math::RectF rect, uint32_t color, Direction dir) {
    float widthOffset = rect.w/5.f;
    float width = rect.w - widthOffset * 2.f;
    float height = width * 0.5f;
    if (height > rect.h) {
        height = rect.h;
        width = width * 2;
        widthOffset = (rect.w - width) * 0.5f;
    }
    float heightOffset = (rect.h - height) * 0.5f;

    rect -= math::RectOffsetF(widthOffset, widthOffset, heightOffset, heightOffset);

    ImVec2 left, right, center;
    if (dir == Direction::Up) {
        left = ToImGui(rect.LeftBottom());
        center = ImVec2(rect.CenterX(), rect.Top());
        right = ToImGui(rect.RightBottom());
    } else {
        left = ToImGui(rect.LeftTop());
        center = ImVec2(rect.CenterX(), rect.Bottom());
        right = ToImGui(rect.RightTop());
    }

    float thickness = 2.f;
    const ImVec2 points[] = {left, center, right};
    drawList->AddPolyline(points, 3, color, false, thickness);
}

} // end namespace gui
