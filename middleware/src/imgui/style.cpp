#include "middleware/imgui/style.h"

#include "imgui/imgui.h"
#include "imgui/internal.h"


namespace gui {

void StartDisable() {
    ImGuiContext& g = *GImGui;
    ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, g.Style.Alpha * 0.5f);
}

void FinishDisable() {
    ImGui::PopStyleVar();
    ImGui::PopItemFlag();
}

} // end namespace gui

