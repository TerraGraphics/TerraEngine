#include "editor/windows/panel_window.h"

#include <utility>

#include "imgui/imgui.h"


Property::Property(Property::Callback&& drawCallback)
    : m_drawCallback(std::move(drawCallback)) {

}

void Property::Draw() {
    if (m_drawCallback) {
        m_drawCallback();
    }
}


PanelWindow::PanelWindow() {

}

PanelWindow::~PanelWindow() {

}

void PanelWindow::Create() {

}

void PanelWindow::SetProperties(std::initializer_list<Property> properties) {
    m_properties = properties;
}

void PanelWindow::Draw() {
    bool* pOpen = nullptr;
    ImGuiWindowFlags windowFlags = 0;
    if (ImGui::Begin("Property", pOpen, windowFlags)) {
        for (auto& property: m_properties) {
            property.Draw();
        }
        ImGui::End();
    }
}
