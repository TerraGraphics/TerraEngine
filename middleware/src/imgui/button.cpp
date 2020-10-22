#include "middleware/imgui/button.h"

#include "imgui/imgui.h"
#include "imgui/internal.h"
#include "imgui/gui_helpers.h"
#include "core/math/constants.h"
#include "middleware/imgui/imgui_math.h"


namespace gui {

void RenderArrow(ImDrawList* drawList, math::PointF pos, float size, uint32_t color, ButtonDir dir) {
    // h = sqrt(3) * 0.5 * size
    const float r = sqrtf(3.f) * 0.25f * size;
    const ImVec2 center = ToImGui(pos) + ImVec2(r, r);

    ImVec2 points[3];
    float angle = (dir == ButtonDir::Up) ? 0 : PI<float>();
    for (ImVec2& p: points) {
        sincosf(angle, &p.x, &p.y);
        p *= r;
        p += center;
        angle += OneThirdOfTwoPI<float>();
    }

    drawList->AddTriangleFilled(points[0], points[1], points[2], color);
}

bool ButtonArrow(std::string_view strId, ButtonDir dir, const ButtonStyle& style, math::RectF* outWidgetRect) {
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems) {
        if (outWidgetRect != nullptr) {
            *outWidgetRect = math::RectF();
        }
        return false;
    }

    ImGuiContext& g = *GImGui;
    const ImGuiID id = window->GetID(strId.cbegin(), strId.cend());
    const math::SizeF drawSize = math::SizeF(g.FontSize);

    math::RectF drawRect;
    math::RectF widgetRect;
    bool res = PlaceWidget(id, static_cast<const Style*>(&style), drawSize, &drawRect, &widgetRect);
    if (outWidgetRect != nullptr) {
        *outWidgetRect = widgetRect;
    }
    if (!res) {
        return false;
    }

    bool hovered, held;
    bool pressed = ImGui::ButtonBehavior(ToImGui(drawRect), id, &hovered, &held, ImGuiButtonFlags_None);
    ImGui::RenderNavHighlight(ToImGui(widgetRect), id);

    if (held || hovered) {
        const uint32_t bgColor = ImGui::GetColorU32((held && hovered) ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
        ImGui::RenderFrame(ToImGui(widgetRect.Min()), ToImGui(widgetRect.Max()), bgColor, true, g.Style.FrameRounding);
    }

    RenderArrow(window->DrawList, drawRect.LeftTop(), g.FontSize, ImGui::GetColorU32(ImGuiCol_Text), dir);

    return pressed;
}

} // end namespace gui
