#include "imgui/gui_helpers.h"

#include "imgui/imgui.h"
#include "imgui/internal.h"
#include "middleware/imgui/imgui_math.h"


namespace gui {

void ItemSize(math::SizeF widgetSize) {
    ImGui::ItemSize(ToImGui(widgetSize), 0);
}

bool ItemAdd(math::RectF widgetRect) {
    return ImGui::ItemAdd(ToImGui(widgetRect), 0);
}

bool PlaceWidget(math::SizeF widgetSize, math::RectF& widgetRect) {
    ImGuiWindow* window = ImGui::GetCurrentWindow();

    const math::PointF widgetPos(window->DC.CursorPos.x, window->DC.CursorPos.y /*+ window->DC.CurrLineTextBaseOffset*/);
    widgetRect = math::RectF(widgetPos, widgetSize);

    ItemSize(widgetSize);
    return ItemAdd(widgetRect);
}

void PlaceWidgetCalc(const Style* style, math::SizeF minSize, math::SizeF drawSize, math::RectF& drawRect, math::RectF& widgetRect) {
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
}

bool PlaceWidget(const Style* style, math::SizeF minSize, math::SizeF drawSize, math::RectF& drawRect, math::RectF& widgetRect) {
    PlaceWidgetCalc(style, minSize, drawSize, drawRect, widgetRect);

    ItemSize(widgetRect.Size());
    return ItemAdd(widgetRect);
}

bool IsRectVisible(math::RectF rect) {
    ImGuiWindow* window = GImGui->CurrentWindow;
    return window->ClipRect.Overlaps(ToImGui(rect));
}

} // end namespace gui
