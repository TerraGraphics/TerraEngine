#include "property_window.h"

#include <imgui.h>


Property::Property(Property::Callback&& drawCallback)
    : m_drawCallback(std::move(drawCallback)) {

}

void Property::Draw() {
    if (m_drawCallback) {
        m_drawCallback();
    }
}


PropertyWindow::PropertyWindow() {

}

PropertyWindow::~PropertyWindow() {

}

void PropertyWindow::Create() {

}

void PropertyWindow::SetProperties(std::initializer_list<Property> properties) {
    m_properties = properties;
}

void PropertyWindow::Draw() {
    bool* pOpen = nullptr;
    ImGuiWindowFlags windowFlags = 0;
    if (ImGui::Begin("Property", pOpen, windowFlags)) {
        for (auto& property: m_properties) {
            property.Draw();
        }
        ImGui::End();
    }
}
