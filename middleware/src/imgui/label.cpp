#include "middleware/imgui/label.h"

#include "imgui/imgui.h"
#include "imgui/internal.h"
#include "imgui/gui_helpers.h"
#include "middleware/imgui/imgui_math.h"


namespace gui {

void SameLine() {
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems) {
        return;
    }

    window->DC.CursorPos.x = window->DC.CursorPosPrevLine.x + g.Style.ItemSpacing.x;
    window->DC.CursorPos.y = window->DC.CursorPosPrevLine.y;
    window->DC.CurrLineSize = window->DC.PrevLineSize;
    window->DC.CurrLineTextBaseOffset = window->DC.PrevLineTextBaseOffset;
}

math::SizeF CalcTextSize(std::string_view text, float wrapWidth) {
    return ToSizeF(ImGui::CalcTextSize(text.cbegin(), text.cend(), false, wrapWidth));
}

math::RectF Label(std::string_view text, const LabelStyle& style, math::SizeF minSize) {
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems) {
        return math::RectF();
    }

    const char* begin = text.cbegin();
    const char* end = text.cend();
    IM_ASSERT(begin != nullptr);

    const float wrapPosX = window->DC.TextWrapPos;
    const bool wrapEnabled = (wrapPosX >= 0.0f);
    const float wrapWidth = wrapEnabled ? ImGui::CalcWrapWidthForPos(window->DC.CursorPos, wrapPosX) : 0.0f;
    const math::SizeF drawSize = CalcTextSize(text, wrapWidth);

    math::RectF drawRect;
    math::RectF widgetRect;
    if (!PlaceWidget(static_cast<const Style*>(&style), minSize, drawSize, drawRect, widgetRect)) {
        return widgetRect;
    }

    if (!text.empty()) {
        ImGuiContext& g = *GImGui;
        window->DrawList->AddText(g.Font, g.FontSize, ToImGui(drawRect.Min()), ImGui::GetColorU32(ImGuiCol_Text), begin, end, wrapWidth);
    }

    return widgetRect;
}

} // end namespace gui
