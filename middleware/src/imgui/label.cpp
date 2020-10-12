#include "middleware/imgui/label.h"

#include "imgui/imgui.h"
#include "imgui/internal.h"


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

void Label(std::string_view text) {
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems) {
        return;
    }

    const char* begin = text.cbegin();
    const char* end = text.cend();
    IM_ASSERT(begin != nullptr);

    const float wrapPosX = window->DC.TextWrapPos;
    const bool wrapEnabled = (wrapPosX >= 0.0f);
    const float wrapWidth = wrapEnabled ? ImGui::CalcWrapWidthForPos(window->DC.CursorPos, wrapPosX) : 0.0f;

    const ImVec2 textPos(window->DC.CursorPos.x, window->DC.CursorPos.y + window->DC.CurrLineTextBaseOffset);
    const ImVec2 textSize = ImGui::CalcTextSize(begin, end, false, wrapWidth);

    ImRect bb(textPos, textPos + textSize);
    ImGui::ItemSize(textSize, 0.0f);
    if (!ImGui::ItemAdd(bb, 0)) {
        return;
    }

    if (!text.empty()) {
        window->DrawList->AddText(g.Font, g.FontSize, textPos, ImGui::GetColorU32(ImGuiCol_Text), begin, end, wrapWidth);
    }
}

} // end namespace gui
