#include "middleware/imgui/label.h"

#include "imgui/imgui.h"
#include "imgui/internal.h"
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

void ItemSize(math::SizeF size) {
    ImGui::ItemSize(ToImGui(size), 0);
}

bool ItemAdd(math::RectF rect) {
    return ImGui::ItemAdd(ToImGui(rect), 0);
}

math::SizeF Label(std::string_view text, math::SizeF minSize, const LabelStyle& style) {
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems) {
        return minSize;
    }

    const char* begin = text.cbegin();
    const char* end = text.cend();
    IM_ASSERT(begin != nullptr);

    const float wrapPosX = window->DC.TextWrapPos;
    const bool wrapEnabled = (wrapPosX >= 0.0f);
    const float wrapWidth = wrapEnabled ? ImGui::CalcWrapWidthForPos(window->DC.CursorPos, wrapPosX) : 0.0f;

    const math::PointF labelPos(window->DC.CursorPos.x, window->DC.CursorPos.y + window->DC.CurrLineTextBaseOffset);
    const math::SizeF labelSize = std::max(CalcTextSize(text, wrapWidth) + style.padding.Size(), minSize);
    const math::RectF labelRect(labelPos, labelSize);
    const math::RectF innerRect = labelRect - style.padding;

    ItemSize(labelSize);
    if (!ItemAdd(labelRect)) {
        return labelSize;
    }

    if (!text.empty()) {
        ImGuiContext& g = *GImGui;
        window->DrawList->AddText(g.Font, g.FontSize, ToImGui(innerRect.Min()), ImGui::GetColorU32(ImGuiCol_Text), begin, end, wrapWidth);
    }

    return labelSize;
}

} // end namespace gui
