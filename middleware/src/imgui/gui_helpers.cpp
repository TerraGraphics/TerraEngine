#include "imgui/gui_helpers.h"

#include "imgui/imgui.h"
#include "imgui/internal.h"
#include "middleware/imgui/imgui_math.h"


namespace gui {

void ItemSize(math::SizeF size) {
    ImGui::ItemSize(ToImGui(size), 0);
}

bool ItemAdd(math::RectF rect) {
    return ImGui::ItemAdd(ToImGui(rect), 0);
}

bool ItemFullAdd(math::RectF rect) {
    ItemSize(rect.Size());
    return ItemAdd(rect);
}

bool PlaceWidget(const Style* style, math::SizeF minSize, math::SizeF drawSize, math::RectF& drawRect, math::RectF& widgetRect) {
    ImGuiWindow* window = ImGui::GetCurrentWindow();

    const math::PointF widgetPos(window->DC.CursorPos.x, window->DC.CursorPos.y /*+ window->DC.CurrLineTextBaseOffset*/);
    const math::SizeF widgetSize(std::max(drawSize + style->padding.Size(), minSize));
    widgetRect = math::RectF(widgetPos, widgetSize);
    drawRect = widgetRect - style->padding;

    switch (style->horisontalAlign) {
    case HorisontalAlign::Center:
        drawRect.x += (drawRect.Width() - drawSize.w) / 2.f;
        break;
    case HorisontalAlign::Right:
        drawRect.x += (drawRect.Width() - drawSize.w);
        break;
    default:
        break;
    }

    switch (style->verticalAlign) {
    case VerticalAlign::Center:
        drawRect.y += (drawRect.Height() - drawSize.h) / 2.f;
        break;
    case VerticalAlign::Bottom:
        drawRect.y += (drawRect.Height() - drawSize.h);
        break;
    default:
        break;
    }

    ItemSize(widgetRect.Size());
    return ItemAdd(widgetRect);
}

bool IsRectVisible(math::RectF rect) {
    ImGuiWindow* window = GImGui->CurrentWindow;
    return window->ClipRect.Overlaps(ToImGui(rect));
}

} // end namespace gui
