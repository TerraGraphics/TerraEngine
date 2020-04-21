#include "graph_window.h"

#include <imgui.h>
#include <imgui_node_editor.h>

#include "core/engine.h"
#include "property_window.h"
#include "core/dg/texture.h"
#include "core/math/types.h"
#include "core/math/constants.h"
#include "middleware/imgui/widgets.h"
#include "core/dg/texture_utilities.h"
#include "middleware/graph_editor/graph_node.h"
#include "middleware/graph_editor/graph_editor.h"
#include "middleware/graph_editor/graph_selected_node.h"
#include "middleware/generator/texture/noise_rasterization.h"
#include "middleware/generator/texture/texture_node_factory.h"


namespace ed = ax::NodeEditor;

GraphWindow::GraphWindow() {

}

GraphWindow::~GraphWindow() {

}

void GraphWindow::Create(const std::shared_ptr<PropertyWindow>& propertyWindow) {
    auto& engine = Engine::Get();
    auto& device = engine.GetDevice();

    m_propertyWindow = propertyWindow;

    TexturePtr texBackground;
    dg::TextureLoadInfo loadInfo;
    loadInfo.IsSRGB = true;
    CreateTextureFromFile("assets/graph_node_background.png", loadInfo, device, &texBackground);

    auto factory = std::make_unique<TextureNodeFactory>(device, engine.GetImmediateContext());
    NoiseToTexture* node = factory->CreateNoiseToTexture();
    m_graphEditor = std::make_shared<GraphEditor>("NodeEditor", device->GetDeviceCaps().IsGLDevice(), texBackground, std::move(factory));
    m_graphEditor->AddNode(static_cast<GraphNode*>(node));
    m_selectedNode = m_graphEditor->GetSelectedNode();

    // TODO: remove
    m_propertyWindow->SetProperties({
        Property([node = m_selectedNode]() {
            node->Draw();
        })
    });
}

void GraphWindow::Draw() {
    bool* pOpen = nullptr;
    ImGuiWindowFlags windowFlags = 0;
    if (ImGui::Begin("graph", pOpen, windowFlags)) {
        m_graphEditor->Draw();
        ImGui::End();
    }
}
