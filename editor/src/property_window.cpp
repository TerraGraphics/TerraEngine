#include "property_window.h"

#include <imgui.h>


PropertyWindow::PropertyWindow() {

}

PropertyWindow::~PropertyWindow() {

}

void PropertyWindow::Create() {

}

void PropertyWindow::Draw() {
    bool* pOpen = nullptr;
    ImGuiWindowFlags windowFlags = 0;
    if (ImGui::Begin("Property", pOpen, windowFlags)) {
        ImGui::End();
    }
}
