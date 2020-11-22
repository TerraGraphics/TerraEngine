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

math::SizeF CalculateDrawSize(math::SizeF drawSize, math::SizeF defaultSize) {
    if (std::fpclassify(drawSize.w) == FP_ZERO) {
        drawSize.w = defaultSize.w;
    }
    if (std::fpclassify(drawSize.h) == FP_ZERO) {
        drawSize.h = defaultSize.h;
    }

    return drawSize;
}

bool RenderBaseButton(const ImGuiID id, math::RectF drawRect, ImGuiButtonFlags flags, bool drawBg) {
    bool hovered, held;
    bool pressed = ImGui::ButtonBehavior(ToImGuiRect(drawRect), id, &hovered, &held, flags);
    ImGui::RenderNavHighlight(ToImGuiRect(drawRect), id);

    if (held || hovered) {
        auto idx = (held && hovered) ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button;
        const uint32_t bgColor = GetThemeColor(idx).value;
        ImGui::RenderFrame(ToImGui(drawRect.Min()), ToImGui(drawRect.Max()), bgColor, true, GImGui->Style.FrameRounding);
    } else if (drawBg) {
        ImGui::RenderFrame(ToImGui(drawRect.Min()), ToImGui(drawRect.Max()), GetThemeColor(ImGuiCol_FrameBg).value, true, GImGui->Style.FrameRounding);
    }

    return pressed;
}

bool CheckBox(std::string_view strId, bool& value, const ButtonStyle& style, math::RectF* outRect) {
    ImGuiWindow* window = GetCheckedCurrentWindow(outRect);
    if (window == nullptr) {
        return false;
    }

    math::RectF drawRect;
    const ImGuiID id = window->GetID(strId.cbegin(), strId.cend());
    math::SizeF drawSize = CalculateDrawSize(style.drawSize, math::SizeF(GetDefaultFieldHeight()));
    if (!WidgetPlace(id, &style, drawSize, &drawRect, outRect)) {
        return false;
    }

    bool pressed = RenderBaseButton(id, drawRect, ImGuiButtonFlags_None, true);
    if (pressed) {
        value = !value;
    }

    if (value) {
        auto color = GetThemeColor(ImGuiCol_CheckMark).value;
        const float pad = std::max(1.0f, std::floor(drawRect.h / 6.0f));
        ImGui::RenderCheckMark(window->DrawList, ToImGui(drawRect.Min()) + ImVec2(pad, pad), color, drawRect.h - pad * 2.0f);
    }
    DrawTooltip(&style);

    return pressed;
}

bool ArrowButton(std::string_view strId, Direction dir, const ButtonStyle& style, math::RectF* outRect) {
    ImGuiWindow* window = GetCheckedCurrentWindow(outRect);
    if (window == nullptr) {
        return false;
    }

    math::RectF drawRect;
    const ImGuiID id = window->GetID(strId.cbegin(), strId.cend());
    math::SizeF drawSize = CalculateDrawSize(style.drawSize, math::SizeF(GetDefaultFieldHeight()));
    if (!WidgetPlace(id, &style, drawSize, &drawRect, outRect)) {
        return false;
    }

    bool pressed = RenderBaseButton(id, drawRect, ImGuiButtonFlags_None, false);
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

    bool pressed = RenderBaseButton(id, drawRect, ImGuiButtonFlags_Repeat, false);
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
