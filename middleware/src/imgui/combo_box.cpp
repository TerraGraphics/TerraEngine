#include "middleware/imgui/combo_box.h"

#include <cstddef>
#include <algorithm>

#include "imgui/imgui.h"
#include "imgui/internal.h"
#include "imgui/widget_draw.h"
#include "core/common/exception.h"
#include "imgui/widget_placement.h"
#include "middleware/imgui/button.h"
#include "middleware/imgui/imgui_math.h"


namespace gui {

static float CalcMaxPopupHeightFromItemCount(uint8_t itemsCount) {
    ImGuiContext& g = *GImGui;
    return (g.FontSize + g.Style.ItemSpacing.y) * static_cast<float>(itemsCount) - g.Style.ItemSpacing.y + (g.Style.WindowPadding.y * 2);
}

bool ComboBox(std::string_view strId, size_t& index, const std::vector<std::string>& items, const ComboBoxStyle& style, math::RectF* outRect) {
    bool changed = false;

    ImGuiWindow* window = GetCheckedCurrentWindow(outRect);
    if (window == nullptr) {
        return changed;
    }

    if (index >= items.size()) {
        throw EngineError("gui::ComboBox: wrong index = {}, max value = {}", index, items.size() - 1);
    }
    const char* previewValue = items[index].c_str();

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& theme = g.Style;
    const ImGuiID id = window->GetID(strId.cbegin(), strId.cend());
    const auto drawSize = math::SizeF(style.width, GetDefaultFieldHeight());
    const auto arrowSize = math::SizeF(GetDefaultFieldHeight());
    // Always consume the SetNextWindowSizeConstraint() call in our early return paths
    bool hasWindowSizeConstraint = (g.NextWindowData.Flags & ImGuiNextWindowDataFlags_HasSizeConstraint) != 0;
    g.NextWindowData.Flags &= ~ImGuiNextWindowDataFlags_HasSizeConstraint;

    math::RectF drawRect;
    math::RectF fullRect;
    bool res = WidgetPlace(id, &style, drawSize, &drawRect, &fullRect);
    if (outRect != nullptr) {
        *outRect = fullRect;
    }
    if (!res) {
        return changed;
    }

    const bool drawArrow = (arrowSize.w <= drawRect.w);
    const math::RectF drawArrowRect = drawRect - math::RectOffsetF(drawRect.w - arrowSize.w, 0, 0, 0);
    const math::RectF drawPreviewRect = drawRect - math::RectOffsetF(theme.FramePadding.x, drawArrow ? arrowSize.w : theme.FramePadding.x, theme.FramePadding.y, theme.FramePadding.y);
    const ImRect drawRectIm = ToImGuiRect(drawRect);
    const uint8_t popupItemCout = 8;
    const ImVec2 popupWindowMinSize = ImVec2(drawRect.w, 0.0f);
    const ImVec2 popupWindowMaxSize = ImVec2(FLT_MAX, CalcMaxPopupHeightFromItemCount(popupItemCout));

    bool hovered, held;
    bool pressed = ImGui::ButtonBehavior(drawRectIm, id, &hovered, &held);
    bool popupOpened = ImGui::IsPopupOpen(id, ImGuiPopupFlags_None);

    ImGui::RenderNavHighlight(drawRectIm, id);
    const auto frameColor = GetThemeColor(hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg).value;
    window->DrawList->AddRectFilled(drawRectIm.Min, drawRectIm.Max, frameColor, theme.FrameRounding, ImDrawCornerFlags_All);
    if (drawArrow) {
        DrawArrowIcon(window->DrawList, drawArrowRect, GetThemeColor(ImGuiCol_Text), Direction::Down);
    }
    if (previewValue != nullptr) {
        ImGui::RenderTextClipped(ToImGui(drawPreviewRect.Min()), ToImGui(drawPreviewRect.Max()), previewValue, NULL, NULL, ImVec2(0.0f, 0.0f));
    }
    ImGui::RenderFrameBorder(drawRectIm.Min, drawRectIm.Max, theme.FrameRounding);

    if ((pressed || g.NavActivateId == id) && !popupOpened) {
        if (window->DC.NavLayerCurrent == 0) {
            window->NavLastIds[0] = id;
        }
        ImGui::OpenPopupEx(id, ImGuiPopupFlags_None);
        popupOpened = true;
    }

    if (!popupOpened) {
        return changed;
    }

    if (hasWindowSizeConstraint) {
        g.NextWindowData.Flags |= ImGuiNextWindowDataFlags_HasSizeConstraint;
        g.NextWindowData.SizeConstraintRect.Min.x = ImMax(g.NextWindowData.SizeConstraintRect.Min.x, drawRect.w);
    } else {
        ImGui::SetNextWindowSizeConstraints(popupWindowMinSize, popupWindowMaxSize);
    }

    char name[16];
    ImFormatString(name, 16, "##Combo_%02d", g.BeginPopupStack.Size); // Recycle windows based on depth

    // Peak into expected window size so we can position it
    if (ImGuiWindow* popupWindow = ImGui::FindWindowByName(name); popupWindow != nullptr && popupWindow->WasActive) {
        ImVec2 expectedSize = ImGui::CalcWindowExpectedSize(popupWindow);
        ImRect outerRect = ImGui::GetWindowAllowedExtentRect(popupWindow);
        ImVec2 pos = ImGui::FindBestWindowPosForPopupEx(drawRectIm.GetBL(), expectedSize, &popupWindow->AutoPosLastDirection, outerRect, drawRectIm, ImGuiPopupPositionPolicy_ComboBox);
        ImGui::SetNextWindowPos(pos);
    }

    const ImGuiWindowFlags windowFlags = ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_Popup |
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoMove;

    // Horizontally align ourselves with the framed text
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(theme.FramePadding.x, theme.WindowPadding.y));
    // We don't use BeginPopupEx() solely because we have a custom name string, which we could make an argument to BeginPopupEx()
    bool isOpened = ImGui::Begin(name, NULL, windowFlags);
    ImGui::PopStyleVar();
    if (!isOpened) {
        ImGui::EndPopup();
        IM_ASSERT(0);   // This should never happen as we tested for IsPopupOpen() above
        return changed;
    }

    for (size_t i=0; i!=items.size(); ++i) {
        bool selected = (i == index);
        if (ImGui::Selectable(items[i].c_str(), selected, ImGuiSelectableFlags_None, ImVec2(0, 0))) {
            changed = (i != index);
            index = i;
            ImGui::SetItemDefaultFocus();
        }
    }
    ImGui::EndPopup();

    return changed;
}

} // end namespace gui
