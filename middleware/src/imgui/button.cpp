#include "middleware/imgui/button.h"

#include <cmath>
#include <string>
#include <cstdint>

#include "imgui/imgui.h"
#include "imgui/internal.h"
#include "imgui/widget_draw.h"
#include "imgui/widget_placement.h"
#include "middleware/imgui/imgui_math.h"


namespace gui {

bool RenderBaseButton(const ImGuiID id, math::RectF drawRect, ImGuiButtonFlags flags) {
    bool hovered, held;
    bool pressed = ImGui::ButtonBehavior(ToImGuiRect(drawRect), id, &hovered, &held, flags);
    ImGui::RenderNavHighlight(ToImGuiRect(drawRect), id);

    if (held || hovered) {
        auto idx = (held && hovered) ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button;
        const uint32_t bgColor = GetThemeColor(idx).value;
        ImGui::RenderFrame(ToImGui(drawRect.Min()), ToImGui(drawRect.Max()), bgColor, true, GImGui->Style.FrameRounding);
    }

    return pressed;
}

bool ArrowButton(std::string_view strId, Direction dir, const ButtonStyle& style, math::RectF* outRect) {
    ImGuiWindow* window = GetCheckedCurrentWindow(outRect);
    if (window == nullptr) {
        return false;
    }

    const ImGuiID id = window->GetID(strId.cbegin(), strId.cend());
    math::SizeF drawSize = style.drawSize;
    if (std::fpclassify(drawSize.w) == FP_ZERO) {
        drawSize.w = GetDefaultFieldHeight();
    }
    if (std::fpclassify(drawSize.h) == FP_ZERO) {
        drawSize.h = GetDefaultFieldHeight();
    }

    math::RectF drawRect;
    if (!WidgetPlace(id, &style, drawSize, &drawRect, outRect)) {
        return false;
    }

    bool pressed = RenderBaseButton(id, drawRect, ImGuiButtonFlags_None);
    DrawArrowIcon(window->DrawList, drawRect, GetThemeColor(ImGuiCol_Text), dir);
    DrawTooltip(&style);

    return pressed;
}

bool StepButton(std::string_view strId, Direction dir, math::RectF drawRect) {
    ImGuiWindow* window = GetCurrentWindow();

    const ImGuiID id = window->GetID(strId.cbegin(), strId.cend());
    if (!ItemAdd(id, drawRect)) {
        return false;
    }

    bool pressed = RenderBaseButton(id, drawRect, ImGuiButtonFlags_Repeat);
    DrawArrowIcon(window->DrawList, drawRect, GetThemeColor(ImGuiCol_Text), dir);

    return pressed;
}

StepButtonAction StepButtons(std::string_view strId, const ButtonStyle& style, math::RectF* outRect) {
    StepButtonAction result = StepButtonAction::None;
    ImGuiWindow* window = GetCheckedCurrentWindow(outRect);
    if (window == nullptr) {
        return result;
    }

    math::SizeF drawSize = style.drawSize;
    if (std::fpclassify(drawSize.h) == FP_ZERO) {
        drawSize.h = GetDefaultFieldHeight();
    }
    if (std::fpclassify(drawSize.w) == FP_ZERO) {
        drawSize.w = GetStepButtonsWidth(drawSize.h);
    }

    math::RectF fullRect;
    math::RectF drawRect = WidgetCalc(&style, drawSize, &fullRect);
    ItemSize(fullRect.Size());

    drawRect.h /= 2.f;
    if (StepButton(std::string(strId) + ".up", Direction::Up, drawRect)) {
        result = StepButtonAction::Up;
    }

    drawRect.y += drawRect.h;
    if (StepButton(std::string(strId) + ".down", Direction::Down, drawRect)) {
        result = StepButtonAction::Down;
    }

    if (outRect != nullptr) {
        *outRect = fullRect;
    }

    DrawTooltip(&style);

    return result;
}

} // end namespace gui
