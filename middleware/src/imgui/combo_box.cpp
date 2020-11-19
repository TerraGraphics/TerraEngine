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

static float CalcMaxPopupHeightFromItemCount(int items_count) {
    ImGuiContext& g = *GImGui;
    if (items_count <= 0)
        return FLT_MAX;
    return (g.FontSize + g.Style.ItemSpacing.y) * static_cast<float>(items_count) - g.Style.ItemSpacing.y + (g.Style.WindowPadding.y * 2);
}

bool ComboBox(std::string_view strId, size_t& index, const std::vector<std::string>& items, const ComboBoxStyle& style, math::RectF* outWidgetRect) {
    bool changed = false;

    ImGuiWindow* window = GetCheckedCurrentWindow(outWidgetRect);
    if (window == nullptr) {
        return changed;
    }

    if (index >= items.size()) {
        throw EngineError("gui::ComboBox: wrong index = {}, max value = {}", index, items.size() - 1);
    }
    const char* previewValue = items[index].c_str();

    ImGuiContext& g = *GImGui;
    const ImGuiID id = window->GetID(strId.cbegin(), strId.cend());
    const auto drawSize = math::SizeF(style.width, GetDefaultFieldHeight());
    // Always consume the SetNextWindowSizeConstraint() call in our early return paths
    bool hasWindowSizeConstraint = (g.NextWindowData.Flags & ImGuiNextWindowDataFlags_HasSizeConstraint) != 0;
    g.NextWindowData.Flags &= ~ImGuiNextWindowDataFlags_HasSizeConstraint;

    math::RectF drawRect;
    math::RectF widgetRect;
    bool res = PlaceWidget(id, &style, drawSize, &drawRect, &widgetRect);
    if (outWidgetRect != nullptr) {
        *outWidgetRect = widgetRect;
    }
    if (!res) {
        return changed;
    }

    ImGuiComboFlags flags = ImGuiComboFlags_None;
    const ImGuiStyle& gStyle = g.Style;

    const float arrow_size = ImGui::GetFrameHeight();
    const float width = drawSize.w;
    const ImRect frame_bb = ToImGuiRect(drawRect);

    bool hovered, held;
    bool pressed = ImGui::ButtonBehavior(frame_bb, id, &hovered, &held);
    bool popup_open = ImGui::IsPopupOpen(id, ImGuiPopupFlags_None);

    const ImU32 frame_col = ImGui::GetColorU32(hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg);
    const float value_x2 = ImMax(frame_bb.Min.x, frame_bb.Max.x - arrow_size);
    ImGui::RenderNavHighlight(frame_bb, id);
    window->DrawList->AddRectFilled(frame_bb.Min, frame_bb.Max, frame_col, gStyle.FrameRounding, ImDrawCornerFlags_Left);
    {
        if (value_x2 + arrow_size - gStyle.FramePadding.x <= frame_bb.Max.x) {
            auto arrowDrawRect = drawRect - math::RectOffsetF(value_x2 - drawRect.x, 0, 0, 0);
            DrawArrowIcon(window->DrawList, arrowDrawRect, GetThemeColor(ImGuiCol_Text), Direction::Down);
        }
    }
    ImGui::RenderFrameBorder(frame_bb.Min, frame_bb.Max, gStyle.FrameRounding);
    if (previewValue != nullptr) {
        ImGui::RenderTextClipped(frame_bb.Min + gStyle.FramePadding, ImVec2(value_x2, frame_bb.Max.y), previewValue, NULL, NULL, ImVec2(0.0f, 0.0f));
    }

    if ((pressed || g.NavActivateId == id) && !popup_open) {
        if (window->DC.NavLayerCurrent == 0) {
            window->NavLastIds[0] = id;
        }
        ImGui::OpenPopupEx(id, ImGuiPopupFlags_None);
        popup_open = true;
    }

    if (!popup_open) {
        return changed;
    }

    if (hasWindowSizeConstraint) {
        g.NextWindowData.Flags |= ImGuiNextWindowDataFlags_HasSizeConstraint;
        g.NextWindowData.SizeConstraintRect.Min.x = ImMax(g.NextWindowData.SizeConstraintRect.Min.x, width);
    } else {
        if ((flags & ImGuiComboFlags_HeightMask_) == 0)
            flags |= ImGuiComboFlags_HeightRegular;
        IM_ASSERT(ImIsPowerOfTwo(flags & ImGuiComboFlags_HeightMask_));    // Only one
        int popup_max_height_in_items = -1;
        if (flags & ImGuiComboFlags_HeightRegular)     popup_max_height_in_items = 8;
        else if (flags & ImGuiComboFlags_HeightSmall)  popup_max_height_in_items = 4;
        else if (flags & ImGuiComboFlags_HeightLarge)  popup_max_height_in_items = 20;
        ImGui::SetNextWindowSizeConstraints(ImVec2(width, 0.0f), ImVec2(FLT_MAX, CalcMaxPopupHeightFromItemCount(popup_max_height_in_items)));
    }

    char name[16];
    ImFormatString(name, 16, "##Combo_%02d", g.BeginPopupStack.Size); // Recycle windows based on depth

    // Peak into expected window size so we can position it
    if (ImGuiWindow* popup_window = ImGui::FindWindowByName(name)) {
        if (popup_window->WasActive) {
            ImVec2 size_expected = ImGui::CalcWindowExpectedSize(popup_window);
            if (flags & ImGuiComboFlags_PopupAlignLeft)
                popup_window->AutoPosLastDirection = ImGuiDir_Left;
            ImRect r_outer = ImGui::GetWindowAllowedExtentRect(popup_window);
            ImVec2 pos = ImGui::FindBestWindowPosForPopupEx(frame_bb.GetBL(), size_expected, &popup_window->AutoPosLastDirection, r_outer, frame_bb, ImGuiPopupPositionPolicy_ComboBox);
            ImGui::SetNextWindowPos(pos);
        }
    }

    // We don't use BeginPopupEx() solely because we have a custom name string, which we could make an argument to BeginPopupEx()
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_Popup | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove;

    // Horizontally align ourselves with the framed text
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(gStyle.FramePadding.x, gStyle.WindowPadding.y));
    bool ret = ImGui::Begin(name, NULL, window_flags);
    ImGui::PopStyleVar();
    if (!ret) {
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
