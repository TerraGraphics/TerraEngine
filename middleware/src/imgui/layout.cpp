#include "middleware/imgui/layout.h"

#include <stack>

#include "imgui/imgui.h"
#include "imgui/internal.h"
#include "core/math/types.h"
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
    if (!PlaceWidget(0, size, &widgetRect)) {
        return widgetRect;
    }

    return widgetRect;
}

static std::stack<ImGuiLayoutType> layouts;

void BeginGroup(bool horizontal) {
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    layouts.push(window->DC.LayoutType);
    window->DC.LayoutType = horizontal ? ImGuiLayoutType_Horizontal : ImGuiLayoutType_Vertical;

    ImGui::BeginGroup();
}

math::RectF EndGroup() {
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    window->DC.LayoutType = layouts.top();
    layouts.pop();

    ImGui::EndGroup();
    return ToRectF(window->DC.LastItemRect);
}

void BeginHorizontal() {
    BeginGroup(true);
}

math::RectF EndHorizontal() {
    return EndGroup();
}

void BeginVertical() {
    BeginGroup(false);
}

math::RectF EndVertical() {
    return EndGroup();
}

} // end namespace gui
