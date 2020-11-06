#include "middleware/imgui/button.h"

#include <cmath>
#include <cstdint>

#include "imgui/imgui.h"
#include "imgui/internal.h"
#include "imgui/gui_helpers.h"
#include "middleware/imgui/imgui_math.h"


namespace gui {

void RenderArrow(ImDrawList* drawList, math::RectF rect, uint32_t color, ButtonDir dir) {
    rect -= math::RectOffsetF(rect.h/8.f);

    const float h = sqrtf(3.f) * 0.5f * rect.h;
    if (dir == ButtonDir::Up) {
        drawList->AddTriangleFilled(ToImGui(rect.LeftBottom()), ImVec2(rect.CenterX(), rect.Bottom() - h), ToImGui(rect.RightBottom()), color);
    } else {
        drawList->AddTriangleFilled(ToImGui(rect.RightTop()), ImVec2(rect.CenterX(), rect.Top() + h), ToImGui(rect.LeftTop()), color);
    }
}

bool ButtonArrow(std::string_view strId, ButtonDir dir, const ButtonStyle& style, math::RectF* outWidgetRect) {
    ImGuiWindow* window = GetCheckedCurrentWindow(outWidgetRect);
    if (window == nullptr) {
        return false;
    }

    ImGuiContext& g = *GImGui;
    const ImGuiID id = window->GetID(strId.cbegin(), strId.cend());
    const math::SizeF drawSize = math::SizeF(g.FontSize);

    math::RectF drawRect;
    math::RectF widgetRect;
    bool res = PlaceWidget(id, &style, drawSize, &drawRect, &widgetRect);
    if (outWidgetRect != nullptr) {
        *outWidgetRect = widgetRect;
    }
    if (!res) {
        return false;
    }

    bool hovered, held;
    bool pressed = ImGui::ButtonBehavior(ToImGuiRect(drawRect), id, &hovered, &held, ImGuiButtonFlags_None);
    ImGui::RenderNavHighlight(ToImGuiRect(widgetRect), id);

    if (held || hovered) {
        ImGuiCol idx = (held && hovered) ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button;
        const uint32_t bgColor = ImGui::GetColorU32(idx);
        ImGui::RenderFrame(ToImGui(widgetRect.Min()), ToImGui(widgetRect.Max()), bgColor, true, g.Style.FrameRounding);
    }

    RenderArrow(window->DrawList, drawRect, ImGui::GetColorU32(static_cast<ImGuiCol>(ImGuiCol_Text)), dir);
    DrawTooltip(&style);

    return pressed;
}

} // end namespace gui
