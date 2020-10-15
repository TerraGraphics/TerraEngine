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

    const math::PointF labelPos(window->DC.CursorPos.x, window->DC.CursorPos.y + window->DC.CurrLineTextBaseOffset);
    const math::SizeF textSize = CalcTextSize(text, wrapWidth);
    const math::SizeF labelSize = std::max(textSize + style.padding.Size(), minSize);
    const math::RectF labelRect(labelPos, labelSize);
    const math::RectF innerRect = labelRect - style.padding;
    math::PointF textPos = innerRect.Min();

    switch (style.horisontalAlign) {
    case HorisontalAlign::Center:
        textPos.x += (innerRect.Width() - textSize.w) / 2.f;
        break;
    case HorisontalAlign::Right:
        textPos.x += (innerRect.Width() - textSize.w);
        break;
    default:
        break;
    }

    switch (style.verticalAlign) {
    case VerticalAlign::Center:
        textPos.y += (innerRect.Height() - textSize.h) / 2.f;
        break;
    case VerticalAlign::Bottom:
        textPos.y += (innerRect.Height() - textSize.h);
        break;
    default:
        break;
    }

    ItemSize(labelSize);
    if (!ItemAdd(labelRect)) {
        return labelRect;
    }

    if (!text.empty()) {
        ImGuiContext& g = *GImGui;
        window->DrawList->AddText(g.Font, g.FontSize, ToImGui(textPos), ImGui::GetColorU32(ImGuiCol_Text), begin, end, wrapWidth);
    }

    return labelRect;
}

} // end namespace gui
