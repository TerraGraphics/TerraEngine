#include "middleware/imgui/label.h"

#include "imgui/imgui.h"
#include "imgui/internal.h"
#include "imgui/gui_helpers.h"
#include "middleware/imgui/imgui_math.h"


namespace gui {

math::SizeF CalcTextSize(std::string_view text, float& wrapWidth) {
    ImGuiWindow* window = ImGui::GetCurrentWindow();

    const float wrapPosX = window->DC.TextWrapPos;
    const bool wrapEnabled = (wrapPosX >= 0.0f);
    wrapWidth = wrapEnabled ? ImGui::CalcWrapWidthForPos(window->DC.CursorPos, wrapPosX) : 0.0f;

    return ToSizeF(ImGui::CalcTextSize(text.cbegin(), text.cend(), false, wrapWidth));
}

math::RectF LabelCalc(std::string_view text, const LabelStyle& style) {
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems) {
        return math::RectF();
    }

    float wrapWidth;
    math::RectF drawRect;
    math::RectF widgetRect;
    PlaceWidgetCalc(static_cast<const Style*>(&style), CalcTextSize(text, wrapWidth), drawRect, widgetRect);

    return widgetRect;
}

math::RectF Label(std::string_view text, const LabelStyle& style) {
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems) {
        return math::RectF();
    }

    const char* begin = text.cbegin();
    const char* end = text.cend();
    IM_ASSERT(begin != nullptr);

    float wrapWidth;
    const math::SizeF drawSize = CalcTextSize(text, wrapWidth);

    math::RectF drawRect;
    math::RectF widgetRect;
    if (!PlaceWidget(0, static_cast<const Style*>(&style), drawSize, drawRect, widgetRect)) {
        return widgetRect;
    }

    if (!text.empty()) {
        ImGuiContext& g = *GImGui;
        window->DrawList->AddText(g.Font, g.FontSize, ToImGui(drawRect.Min()), ImGui::GetColorU32(ImGuiCol_Text), begin, end, wrapWidth);
    }

    return widgetRect;
}

} // end namespace gui
