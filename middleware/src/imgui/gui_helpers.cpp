#include "imgui/gui_helpers.h"

#include "dg/dg.h"
#include "dg/device.h"
#include "core/engine.h"
#include "imgui/imgui.h"
#include "imgui/internal.h"
#include "middleware/imgui/style.h"
#include "middleware/imgui/imgui_math.h"


namespace gui {

bool isOpenGL() {
    bool isOpenGL = Engine::Get().GetDevice()->GetDeviceCaps().IsGLDevice();

    return isOpenGL;
}

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
    return ImGui::ItemAdd(ToImGui(widgetRect), id);
}

bool PlaceWidget(uint32_t id, math::SizeF widgetSize, math::RectF* outWidgetRect) {
    ImGuiWindow* window = ImGui::GetCurrentWindow();

    const math::PointF widgetPos(window->DC.CursorPos.x, window->DC.CursorPos.y /*+ window->DC.CurrLineTextBaseOffset*/);
    const math::RectF widgetRect(widgetPos, widgetSize);
    if (outWidgetRect != nullptr) {
        *outWidgetRect = widgetRect;
    }

    ItemSize(widgetSize);
    return ItemAdd(id, widgetRect);
}

void PlaceWidgetCalc(const Style* style, math::SizeF drawSize, math::RectF* outDrawRect, math::RectF* outWidgetRect, math::RectF* outFullRect) {
    ImGuiWindow* window = ImGui::GetCurrentWindow();

    const math::PointF fullPos(window->DC.CursorPos.x, window->DC.CursorPos.y /*+ window->DC.CurrLineTextBaseOffset*/);
    const math::SizeF widgetSize(std::max(drawSize + style->padding.Size(), style->minSize));
    const math::SizeF fullSize(widgetSize + style->margin.Size());
    const math::RectF fullRect(fullPos, fullSize);
    const math::RectF widgetRect(fullRect - style->margin);
    math::RectF innerRect(widgetRect - style->padding);

    switch (style->horisontalAlign) {
    case HorisontalAlign::Center:
        innerRect.x += (innerRect.Width() - drawSize.w) / 2.f;
        break;
    case HorisontalAlign::Right:
        innerRect.x += (innerRect.Width() - drawSize.w);
        break;
    default:
        break;
    }

    switch (style->verticalAlign) {
    case VerticalAlign::Center:
        innerRect.y += (innerRect.Height() - drawSize.h) / 2.f;
        break;
    case VerticalAlign::Bottom:
        innerRect.y += (innerRect.Height() - drawSize.h);
        break;
    default:
        break;
    }

    if (outDrawRect != nullptr) {
        *outDrawRect = math::RectF(innerRect.LeftTop(), drawSize);
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
    return window->ClipRect.Overlaps(ToImGui(rect));
}

} // end namespace gui
