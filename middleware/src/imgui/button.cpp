#include "middleware/imgui/button.h"

#include <cmath>
#include <cstdint>

#include "imgui/imgui.h"
#include "imgui/internal.h"
#include "imgui/gui_helpers.h"
#include "middleware/imgui/layout.h"
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

void RenderStepArrow(ImDrawList* drawList, math::RectF rect, uint32_t color, ButtonDir dir) {
    float heightOffset = rect.h/4.f;
    float widthOffset = rect.w/6.f;
    rect -= math::RectOffsetF(widthOffset, widthOffset, heightOffset, heightOffset);

    ImVec2 left, right, center;
    if (dir == ButtonDir::Up) {
        left = ToImGui(rect.LeftBottom());
        center = ImVec2(rect.CenterX(), rect.Top());
        right = ToImGui(rect.RightBottom());
    } else {
        left = ToImGui(rect.LeftTop());
        center = ImVec2(rect.CenterX(), rect.Bottom());
        right = ToImGui(rect.RightTop());
    }

    float thickness = 2.f;
    auto dt = right - center;
    dt /= dt.x * thickness;

    drawList->PathLineTo(left);
    drawList->PathLineTo(center);
    drawList->PathLineTo(right + dt);
    drawList->PathStroke(color, false, thickness);
}

bool RenderBaseButton(const ImGuiID id, math::RectF drawRect, math::RectF widgetRect, ImGuiButtonFlags flags) {
    bool hovered, held;
    bool pressed = ImGui::ButtonBehavior(ToImGuiRect(drawRect), id, &hovered, &held, flags);
    ImGui::RenderNavHighlight(ToImGuiRect(widgetRect), id);

    if (held || hovered) {
        ImGuiCol idx = (held && hovered) ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button;
        const uint32_t bgColor = ImGui::GetColorU32(idx);
        ImGuiContext& g = *GImGui;
        ImGui::RenderFrame(ToImGui(widgetRect.Min()), ToImGui(widgetRect.Max()), bgColor, true, g.Style.FrameRounding);
    }

    return pressed;
}

bool ArrowButton(std::string_view strId, ButtonDir dir, const ButtonStyle& style, math::RectF* outWidgetRect) {
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

    bool pressed = RenderBaseButton(id, drawRect, widgetRect, ImGuiButtonFlags_None);
    RenderArrow(window->DrawList, drawRect, ImGui::GetColorU32(static_cast<ImGuiCol>(ImGuiCol_Text)), dir);
    DrawTooltip(&style);

    return pressed;
}

bool StepButton(std::string_view strId, ButtonDir dir, const ButtonStyle& style) {
    ImGuiWindow* window = GetCurrentWindow();

    const ImGuiID id = window->GetID(strId.cbegin(), strId.cend());

    math::RectF drawRect;
    math::RectF widgetRect;
    if (!PlaceWidget(id, &style, style.minWidgetSize, &drawRect, &widgetRect)) {
        return false;
    }

    bool pressed = RenderBaseButton(id, drawRect, widgetRect, ImGuiButtonFlags_Repeat);
    RenderStepArrow(window->DrawList, drawRect, ImGui::GetColorU32(static_cast<ImGuiCol>(ImGuiCol_Text)), dir);

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
    halfStyle.padding.bottom = 0;
    if (StepButton(std::string(strId) + ".up", ButtonDir::Up, halfStyle)) {
        result = StepButtonAction::Up;
    }

    window->DC.CursorPos.x = backupPosX;
    halfStyle.margin.top = 0;
    halfStyle.margin.bottom = style.margin.bottom;
    halfStyle.padding.top = 0;
    halfStyle.padding.bottom = style.padding.bottom;
    if (StepButton(std::string(strId) + ".down", ButtonDir::Down, halfStyle)) {
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
