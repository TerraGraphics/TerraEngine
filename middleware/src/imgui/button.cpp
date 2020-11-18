#include "middleware/imgui/button.h"

#include <cmath>
#include <string>
#include <cstdint>

#include "imgui/imgui.h"
#include "imgui/internal.h"
#include "imgui/widget_draw.h"
#include "imgui/widget_placement.h"
#include "middleware/imgui/layout.h"
#include "middleware/imgui/imgui_math.h"


namespace gui {

bool RenderBaseButton(const ImGuiID id, math::RectF drawRect, math::RectF widgetRect, ImGuiButtonFlags flags) {
    bool hovered, held;
    bool pressed = ImGui::ButtonBehavior(ToImGuiRect(drawRect), id, &hovered, &held, flags);
    ImGui::RenderNavHighlight(ToImGuiRect(widgetRect), id);

    if (held || hovered) {
        auto idx = (held && hovered) ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button;
        const uint32_t bgColor = GetThemeColor(idx).value;
        ImGui::RenderFrame(ToImGui(widgetRect.Min()), ToImGui(widgetRect.Max()), bgColor, true, GImGui->Style.FrameRounding);
    }

    return pressed;
}

bool ArrowButton(std::string_view strId, Direction dir, const ButtonStyle& style, math::RectF* outWidgetRect) {
    ImGuiWindow* window = GetCheckedCurrentWindow(outWidgetRect);
    if (window == nullptr) {
        return false;
    }

    const ImGuiID id = window->GetID(strId.cbegin(), strId.cend());
    const math::SizeF drawSize = math::SizeF(GetDefaultFieldHeight());

    math::RectF drawRect;
    math::RectF widgetRect;
    bool res = PlaceWidget(id, &style, drawSize, &drawRect, &widgetRect);
    if (outWidgetRect != nullptr) {
        *outWidgetRect = widgetRect;
    }
    if (!res) {
        return false;
    }

    bool pressed = RenderBaseButton(id, drawRect, widgetRect, ImGuiButtonFlags_None);
    DrawArrowIcon(window->DrawList, drawRect, GetThemeColor(ImGuiCol_Text), dir);
    DrawTooltip(&style);

    return pressed;
}

bool StepButton(std::string_view strId, Direction dir, const ButtonStyle& style) {
    ImGuiWindow* window = GetCurrentWindow();

    const ImGuiID id = window->GetID(strId.cbegin(), strId.cend());

    math::RectF drawRect;
    math::RectF widgetRect;
    if (!PlaceWidget(id, &style, style.minWidgetSize, &drawRect, &widgetRect)) {
        return false;
    }

    bool pressed = RenderBaseButton(id, drawRect, widgetRect, ImGuiButtonFlags_Repeat);
    DrawArrowIcon(window->DrawList, drawRect, GetThemeColor(ImGuiCol_Text), dir);

    return pressed;
}

StepButtonAction StepButtons(std::string_view strId, const ButtonStyle& style, math::RectF* outWidgetRect) {
    StepButtonAction result = StepButtonAction::None;
    ImGuiWindow* window = GetCheckedCurrentWindow(outWidgetRect);
    if (window == nullptr) {
        return result;
    }

    float drawHeight = ((style.minWidgetSize.h <= 1.f) ? GetDefaultFieldHeight() : style.minWidgetSize.h);
    float drawWidht = (style.minWidgetSize.w <= 1.f) ? GetStepButtonsWidth(drawHeight) : style.minWidgetSize.h;

    ButtonStyle halfStyle = style;
    halfStyle.minWidgetSize.h = drawHeight * 0.5f;
    halfStyle.minWidgetSize.w = drawWidht;

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
    BeginVertical();

    float backupPosX = window->DC.CursorPos.x;
    halfStyle.margin.bottom = 0;
    if (StepButton(std::string(strId) + ".up", Direction::Up, halfStyle)) {
        result = StepButtonAction::Up;
    }

    window->DC.CursorPos.x = backupPosX;
    halfStyle.margin.top = 0;
    halfStyle.margin.bottom = style.margin.bottom;
    if (StepButton(std::string(strId) + ".down", Direction::Down, halfStyle)) {
        result = StepButtonAction::Down;
    }
    auto widgetRect = EndVertical();
    ImGui::PopStyleVar(1);

    if (outWidgetRect != nullptr) {
        *outWidgetRect = widgetRect;
    }

    DrawTooltip(&style);

    return result;
}

} // end namespace gui
