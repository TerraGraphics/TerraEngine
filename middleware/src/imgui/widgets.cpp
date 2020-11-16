#include "middleware/imgui/widgets.h"

#include "imgui/imgui.h"


namespace gui::detail {

bool Combo(const char* label, size_t& currentIndex, const char* const* itemNames, const size_t numberItems) {
    bool changed = false;
    const char* itemCurrent = itemNames[currentIndex];
    if (ImGui::BeginCombo(label, itemCurrent, ImGuiComboFlags(0))) {
        for (size_t i=0; i!=numberItems; ++i) {
            bool selected = (currentIndex == i);
            if (ImGui::Selectable(itemNames[i], selected)) {
                changed = (i != currentIndex);
                currentIndex = i;
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    return changed;
}

} // end namespace gui::detail
