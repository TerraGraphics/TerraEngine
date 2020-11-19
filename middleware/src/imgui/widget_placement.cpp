#include "imgui/widget_placement.h"

#include <string_view>

#include "imgui/imgui.h"
#include "imgui/internal.h"
#include "middleware/imgui/style.h"
#include "middleware/imgui/imgui_math.h"


namespace gui {

ImGuiWindow* GetCurrentWindow() {
    return ImGui::GetCurrentWindow();
}

ImGuiWindow* GetCheckedCurrentWindow(math::RectF* outWidgetRect) {
    if (ImGuiWindow* window = ImGui::GetCurrentWindow(); !window->SkipItems) {
        return window;
    }

    if (outWidgetRect != nullptr) {
        *outWidgetRect = math::RectF();
    }

    return nullptr;
}

void ItemSize(math::SizeF widgetSize) {
    ImGui::ItemSize(ToImGui(widgetSize), 0);
}

bool ItemAdd(uint32_t id, math::RectF widgetRect) {
    return ImGui::ItemAdd(ToImGuiRect(widgetRect), id);
}

bool PlaceWidget(uint32_t id, math::SizeF widgetSize, math::RectF* outWidgetRect) {
    ImGuiWindow* window = GetCurrentWindow();

    const math::PointF widgetPos(window->DC.CursorPos.x, window->DC.CursorPos.y /*+ window->DC.CurrLineTextBaseOffset*/);
    const math::RectF widgetRect(widgetPos, widgetSize);
    if (outWidgetRect != nullptr) {
        *outWidgetRect = widgetRect;
    }

    ItemSize(widgetSize);
    return ItemAdd(id, widgetRect);
}

void PlaceWidgetCalc(const Style* style, math::SizeF drawSize, math::RectF* outDrawRect, math::RectF* outWidgetRect, math::RectF* outFullRect) {
    ImGuiWindow* window = GetCurrentWindow();

    const math::PointF fullPos(window->DC.CursorPos.x, window->DC.CursorPos.y /*+ window->DC.CurrLineTextBaseOffset*/);

    const math::SizeF maxFullSize(ToSizeF(ImGui::GetContentRegionAvail()));
    const math::SizeF maxWidgetSize(std::max(math::SizeF(0, 0), maxFullSize - style->margin.Size()));
    const math::SizeF maxInnerSize(std::max(math::SizeF(0, 0), maxWidgetSize - style->padding.Size()));

    const math::SizeF innerSize(std::min(maxInnerSize, std::max(drawSize, style->minWidgetSize - style->padding.Size())));
    const math::SizeF widgetSize(std::min(maxWidgetSize, innerSize + style->padding.Size()));
    const math::SizeF fullSize(std::min(maxFullSize, widgetSize + style->margin.Size()));

    const math::RectF fullRect(fullPos, fullSize);
    const math::RectF widgetRect(fullRect - math::RectOffsetF(style->margin, fullSize - widgetSize));
    const math::RectF innerRect(widgetRect - math::RectOffsetF(style->padding, widgetSize - innerSize));

    auto drawRect = math::RectF(innerRect.LeftTop(), std::min(drawSize, innerSize));
    switch (style->horisontalAlign) {
    case HorisontalAlign::Center:
        drawRect.x += (innerRect.Width() - drawRect.w) / 2.f;
        break;
    case HorisontalAlign::Right:
        drawRect.x += (innerRect.Width() - drawRect.w);
        break;
    default:
        break;
    }

    switch (style->verticalAlign) {
    case VerticalAlign::Center:
        drawRect.y += (innerRect.Height() - drawRect.h) / 2.f;
        break;
    case VerticalAlign::Bottom:
        drawRect.y += (innerRect.Height() - drawRect.h);
        break;
    default:
        break;
    }

    if (outDrawRect != nullptr) {
        *outDrawRect = drawRect;
    }
    if (outWidgetRect != nullptr) {
        *outWidgetRect = widgetRect;
    }
    if (outFullRect != nullptr) {
        *outFullRect = fullRect;
    }
}

bool PlaceWidget(uint32_t id, const Style* style, math::SizeF drawSize, math::RectF* outDrawRect, math::RectF* outWidgetRect) {
    math::RectF fullRect;
    math::RectF widgetRect;
    PlaceWidgetCalc(style, drawSize, outDrawRect, &widgetRect, &fullRect);

    if (outWidgetRect != nullptr) {
        *outWidgetRect = widgetRect;
    }

    ItemSize(fullRect.Size());
    return ItemAdd(id, widgetRect);
}

bool IsRectVisible(math::RectF rect) {
    ImGuiWindow* window = GImGui->CurrentWindow;
    return window->ClipRect.Overlaps(ToImGuiRect(rect));
}

} // end namespace gui
