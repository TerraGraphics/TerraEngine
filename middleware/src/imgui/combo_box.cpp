#include "middleware/imgui/combo_box.h"

#include <cstddef>
#include <algorithm>

#include "imgui/imgui.h"
#include "imgui/internal.h"
#include "imgui/gui_helpers.h"
#include "core/common/exception.h"


namespace gui {

bool ComboBox(std::string_view /* strId */, size_t& index, const std::vector<std::string>& items, const ComboBoxStyle& /* style */, math::RectF* /* outWidgetRect */) {
    if (index >= items.size()) {
        throw EngineError("gui::ComboBox: wrong index = {}, max value = {}", index, items.size() - 1);
    }

    bool changed = false;
    if (ImGui::BeginCombo("label", items[index].c_str(), ImGuiComboFlags_None)) {
        for (size_t i=0; i!=items.size(); ++i) {
            bool selected = (i == index);
            if (ImGui::Selectable(items[i].c_str(), selected, ImGuiSelectableFlags_None, ImVec2(0, 0))) {
                changed = (i != index);
                index = i;
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    return changed;
}

} // end namespace gui
