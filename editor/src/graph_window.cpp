#include "graph_window.h"

#include <imgui.h>

#include "core/math/types.h"
#include "core/math/constants.h"
#include "middleware/imgui/widgets.h"



void GraphWindow::Create() {

}

void GraphWindow::Update(double /* deltaTime */) {
    bool* pOpen = nullptr;
    ImGuiWindowFlags windowFlags = 0;
    if (ImGui::Begin("graph", pOpen, windowFlags)) {

        ImGui::End();
    }
}

void GraphWindow::Draw() {

}
