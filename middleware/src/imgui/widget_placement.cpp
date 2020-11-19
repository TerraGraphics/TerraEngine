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

math::RectF WidgetCalc(const Style* style, math::SizeF drawSize, math::RectF* outFullRect) {
    ImGuiWindow* window = GetCurrentWindow();

    math::RectOffsetF margin = std::max(math::RectOffsetF(), style->margin);

    const math::PointF fullPos(window->DC.CursorPos.x, window->DC.CursorPos.y /*+ window->DC.CurrLineTextBaseOffset*/);
    const math::SizeF maxFullSize(ToSizeF(ImGui::GetContentRegionAvail()));
    math::SizeF fullSize(std::min(maxFullSize, style->availableSize));
    if (isnan(style->availableSize.w)) {
        fullSize.w = std::min(maxFullSize.w, drawSize.w + margin.Horizontal());
    } else if (style->availableSize.w < 0.f) {
        fullSize.w = std::max(0.f, maxFullSize.w + style->availableSize.w);
    }
    if (isnan(style->availableSize.h)) {
        fullSize.h = std::min(maxFullSize.h, drawSize.h + margin.Vertical());
    } else if (style->availableSize.h < 0.f) {
        fullSize.h = std::max(0.f, maxFullSize.h + style->availableSize.h);
    }
    const math::RectF fullRect(fullPos, fullSize);

    math::RectF innerRect(fullRect - style->margin);
    innerRect.w = std::max(0.f, innerRect.w);
    innerRect.h = std::max(0.f, innerRect.h);

    auto drawRect = math::RectF(innerRect.LeftTop(), std::max(math::SizeF(), std::min(drawSize, innerRect.Size())));
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

    if (outFullRect != nullptr) {
        *outFullRect = fullRect;
    }

    return drawRect;
}

bool WidgetPlace(uint32_t id, const Style* style, math::SizeF drawSize, math::RectF* outDrawRect, math::RectF* outFullRect) {
    math::RectF fullRect;
    math::RectF drawRect = WidgetCalc(style, drawSize, &fullRect);

    if (fullRect.w < 1.f) {
        drawRect = math::RectF();
        fullRect = math::RectF();
    }

    if (outDrawRect != nullptr) {
        *outDrawRect = drawRect;
    }
    if (outFullRect != nullptr) {
        *outFullRect = fullRect;
    }

    if (fullRect.w < 1.f) {
        return false;
    }

    ItemSize(fullRect.Size());
    return ItemAdd(id, drawRect);
}

bool IsRectVisible(math::RectF rect) {
    ImGuiWindow* window = GImGui->CurrentWindow;
    return window->ClipRect.Overlaps(ToImGuiRect(rect));
}

} // end namespace gui
