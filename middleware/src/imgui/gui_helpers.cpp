#include "middleware/imgui/gui_helpers.h"

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

bool IsRectVisible(math::RectF rect) {
    ImGuiWindow* window = GImGui->CurrentWindow;
    return window->ClipRect.Overlaps(ToImGui(rect));
}

} // end namespace gui
