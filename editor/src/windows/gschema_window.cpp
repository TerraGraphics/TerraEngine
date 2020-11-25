#include "editor/windows/gschema_window.h"

#include "dg/dg.h"
#include "imgui/imgui.h"
#include "core/engine.h"
#include "dg/texture_utilities.h"
#include "editor/windows/property_window.h"
#include "middleware/gschema/editor/gs_editor.h"
#include "middleware/gschema/graph/gs_class_storage.h"


GSchemaWindow::GSchemaWindow() {

}

GSchemaWindow::~GSchemaWindow() {

}

void GSchemaWindow::Create(const std::shared_ptr<PropertyWindow>& propertyWindow) {
    auto& engine = Engine::Get();
    auto& device = engine.GetDevice();

    m_propertyWindow = propertyWindow;

    TexturePtr texBackground;
    dg::TextureLoadInfo loadInfo;
    loadInfo.IsSRGB = true;
    CreateTextureFromFile("assets/graph_node_background.png", loadInfo, device, &texBackground);

    auto classStorage = std::make_shared<gs::ClassStorage>();
    m_editor = std::make_shared<gs::Editor>(classStorage, "GSEditor", texBackground);
    m_editor->Create();
}

void GSchemaWindow::Draw() {
    bool* pOpen = nullptr;
    ImGuiWindowFlags windowFlags = 0;
    if (ImGui::Begin("gseditor", pOpen, windowFlags)) {
        m_editor->DrawGraph();
        ImGui::End();

        m_propertyWindow->SetProperties({
            Property([editor = m_editor]() {
                editor->DrawNodeProperty();
            })
        });
    }
}
