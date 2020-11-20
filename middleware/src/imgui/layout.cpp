#include "middleware/imgui/layout.h"

#include <stack>

#include "imgui/imgui.h"
#include "imgui/internal.h"
#include "core/math/types.h"
#include "middleware/imgui/style.h"
#include "imgui/widget_placement.h"
#include "middleware/imgui/imgui_math.h"


namespace gui {

void SameLine() {
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = GetCheckedCurrentWindow();
    if (window == nullptr) {
        return;
    }

    window->DC.CursorPos.x = window->DC.CursorPosPrevLine.x + g.Style.ItemSpacing.x;
    window->DC.CursorPos.y = window->DC.CursorPosPrevLine.y;
    window->DC.CurrLineSize = window->DC.PrevLineSize;
    window->DC.CurrLineTextBaseOffset = window->DC.PrevLineTextBaseOffset;
}

math::RectF Dummy(math::SizeF size) {
    ImGuiWindow* window = GetCheckedCurrentWindow();
    if (window == nullptr) {
        return math::RectF();
    }

    Style style;
    math::RectF drawRect;
    math::RectF fullRect;
    if (!WidgetPlace(0, &style, size, &drawRect, &fullRect)) {
        return math::RectF();
    }

    return fullRect;
}

static std::stack<ImGuiLayoutType> layouts;

void BeginGroup(bool horizontal) {
    ImGuiWindow* window = GetCurrentWindow();
    layouts.push(window->DC.LayoutType);
    window->DC.LayoutType = horizontal ? ImGuiLayoutType_Horizontal : ImGuiLayoutType_Vertical;

    ImGui::BeginGroup();
}

math::RectF EndGroup() {
    ImGuiWindow* window = GetCurrentWindow();
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
