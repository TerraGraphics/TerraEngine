#include "middleware/imgui/icon.h"

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

math::RectF Dummy(math::SizeF size) {
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems) {
        return math::RectF();
    }

    math::RectF widgetRect;
    if (!PlaceWidget(size, widgetRect)) {
        return widgetRect;
    }

    return widgetRect;
}

void BeginGroup() {
    ImGui::BeginGroup();
}

math::RectF EndGroup() {
    ImGui::EndGroup();

    ImGuiWindow* window = ImGui::GetCurrentWindowRead();
    return ToRectF(window->DC.LastItemRect);
}

} // end namespace gui
