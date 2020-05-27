#include "gschema_window.h"

#include "core/dg/dg.h"
#include "core/engine.h"
#include "property_window.h"
#include "middleware/imgui/imgui.h"
#include "core/dg/texture_utilities.h"
#include "middleware/gscheme/editor/gs_editor.h"


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

    m_editor = std::make_shared<GSEditor>("GSEditor", texBackground);
    m_editor->Create();
}

void GSchemaWindow::Draw() {
    bool* pOpen = nullptr;
    ImGuiWindowFlags windowFlags = 0;
    if (ImGui::Begin("gseditor", pOpen, windowFlags)) {
        m_editor->Draw();
        ImGui::End();

        m_propertyWindow->SetProperties({m_editor->DrawProperty()});
    }
}
