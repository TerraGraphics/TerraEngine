#include "middleware/imgui/label.h"

#include "imgui/imgui.h"
#include "imgui/internal.h"
#include "imgui/gui_helpers.h"
#include "middleware/imgui/imgui_math.h"


namespace gui {

math::SizeF CalcTextSize(std::string_view text, float& wrapWidth) {
    ImGuiWindow* window = GetCurrentWindow();

    const float wrapPosX = window->DC.TextWrapPos;
    const bool wrapEnabled = (wrapPosX >= 0.0f);
    wrapWidth = wrapEnabled ? ImGui::CalcWrapWidthForPos(window->DC.CursorPos, wrapPosX) : 0.0f;

    return ToSizeF(ImGui::CalcTextSize(text.cbegin(), text.cend(), false, wrapWidth));
}

math::RectF LabelCalc(std::string_view text, const LabelStyle& style) {
    ImGuiWindow* window = GetCheckedCurrentWindow();
    if (window == nullptr) {
        return math::RectF();
    }

    float wrapWidth;
    math::RectF fullRect;
    math::RectF widgetRect;
    PlaceWidgetCalc(static_cast<const Style*>(&style), CalcTextSize(text, wrapWidth), nullptr, &widgetRect, &fullRect);

    return widgetRect;
}

void Label(std::string_view text, const LabelStyle& style, math::RectF* outWidgetRect) {
    ImGuiWindow* window = GetCheckedCurrentWindow(outWidgetRect);
    if (window == nullptr) {
        return;
    }

    const char* begin = text.cbegin();
    const char* end = text.cend();
    IM_ASSERT(begin != nullptr);

    float wrapWidth;
    const math::SizeF drawSize = CalcTextSize(text, wrapWidth);

    math::RectF drawRect;
    if (!PlaceWidget(0, &style, drawSize, &drawRect, outWidgetRect)) {
        return;
    }

    if (!text.empty()) {
        ImGuiContext& g = *GImGui;
        const auto clipRect = ToImGui(drawRect);
        const auto startPos = ToImGui(drawRect.Min());
        window->DrawList->AddText(g.Font, g.FontSize, startPos, ImGui::GetColorU32(ImGuiCol_Text), begin, end, wrapWidth, &clipRect);
    }

    DrawTooltip(&style);
}

} // end namespace gui
