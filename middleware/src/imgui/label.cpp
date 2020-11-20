#include "middleware/imgui/label.h"

#include "imgui/imgui.h"
#include "imgui/internal.h"
#include "imgui/widget_draw.h"
#include "imgui/widget_placement.h"
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
    WidgetCalc(&style, CalcTextSize(text, wrapWidth), &fullRect);

    return fullRect;
}

void Label(std::string_view text, const LabelStyle& style, math::RectF* outRect) {
    ImGuiWindow* window = GetCheckedCurrentWindow(outRect);
    if (window == nullptr) {
        return;
    }

    const char* begin = text.cbegin();
    const char* end = text.cend();
    IM_ASSERT(begin != nullptr);

    float wrapWidth;
    const math::SizeF drawSize = CalcTextSize(text, wrapWidth);

    math::RectF drawRect;
    if (!WidgetPlace(0, &style, drawSize, &drawRect, outRect)) {
        return;
    }

    if (!text.empty()) {
        ImGuiContext& g = *GImGui;
        const auto clipRect = ToImGui(drawRect);
        const auto startPos = ToImGui(drawRect.Min());
        window->DrawList->AddText(g.Font, g.FontSize, startPos, GetThemeColor(ImGuiCol_Text).value, begin, end, wrapWidth, &clipRect);
    }

    DrawTooltip(&style);
}

} // end namespace gui
