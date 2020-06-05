#include "middleware/gscheme/editor/gs_editor.h"

#include <utility>
#include <string_view>

#include "middleware/imgui/imgui.h"
#include "middleware/gscheme/graph/gs_id.h"
#include "middleware/gscheme/editor/gs_draw.h"
#include "middleware/gscheme/graph/gs_graph.h"
#include "middleware/imgui/imgui_node_editor.h"
#include "middleware/gscheme/graph/gs_type_class.h"

namespace gs {

Editor::Editor(const std::string& name, TexturePtr& texBackground)
    : m_name(name)
    , m_config(new ne::Config())
    , m_draw(new Draw(texBackground))
    , m_graph(new Graph(16)) {

}

Editor::~Editor() {
    if (m_context) {
        ne::DestroyEditor(m_context);
        m_context = nullptr;
    }

    if (m_config) {
        delete m_config;
        m_config = nullptr;
    }

    m_draw.reset();
    m_graph.reset();
}

void Editor::Create() {
    m_config->SettingsFile = "";
    m_context = ne::CreateEditor(m_config);
    m_graph->AddNode("Sum");
    m_graph->AddNode("Constant");
}

void Editor::DrawGraph() {
    ne::SetCurrentEditor(m_context);
    ne::Begin(m_name.c_str());

    m_graph->DrawGraph(m_draw.get());

    if (const auto id = ne::GetDoubleClickedNode().Get(); id != 0) {
        m_selectedNodeId = static_cast<uint16_t>(id);
    } else if (ne::IsBackgroundClicked()) {
        m_selectedNodeId = 0;
    }

    if (ne::BeginCreate()) {
        ne::PinId srcPin, dstPin;
        if (ne::QueryNewLink(&srcPin, &dstPin)) {
            auto srcPinId = static_cast<uint32_t>(srcPin.Get());
            auto dstPinId = static_cast<uint32_t>(dstPin.Get());
            if (IsInputFromPinId(srcPinId)) {
                std::swap(srcPinId, dstPinId);
            }

            if (!m_graph->TestAddLink(srcPinId, dstPinId)) {
                ne::RejectNewItem();
            } else if (ne::AcceptNewItem()) {
                m_graph->AddLink(srcPinId, dstPinId);
            }
        }
        ne::EndCreate();
    }

    if (ne::BeginDelete()) {
        ne::LinkId link = 0;
        while (ne::QueryDeletedLink(&link)) {
            auto linkId = static_cast<uint64_t>(link.Get());
            if (!m_graph->TestRemoveLink(linkId)) {
                ne::RejectDeletedItem();
            } else if (ne::AcceptDeletedItem()) {
                m_graph->RemoveLink(linkId);
            }
        }

        ne::NodeId node = 0;
        while (ne::QueryDeletedNode(&node)) {
            auto nodeId = static_cast<uint16_t>(node.Get());
            if (!m_graph->TestRemoveNode(nodeId)) {
                ne::RejectDeletedItem();
            } else if (ne::AcceptDeletedItem()) {
                if (m_selectedNodeId == nodeId) {
                    m_selectedNodeId = 0;
                }
                m_graph->RemoveNode(nodeId);
            }
        }
        ne::EndDelete();
    }

    auto currentCursorPosition = ImGui::GetMousePos();
    static auto openPopupPosition = ImGui::GetMousePos();

    ne::Suspend();

    if (ne::ShowBackgroundContextMenu()) {
        ImGui::OpenPopup("Create New Node");
        openPopupPosition = currentCursorPosition;
    }

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
    if (ImGui::BeginPopup("Create New Node")) {
        DrawNewNodeMenu(openPopupPosition.x, openPopupPosition.y);
        ImGui::EndPopup();
    }
    ImGui::PopStyleVar();

    ne::Resume();

    m_graph->UpdateState();

    ne::End();
}

void Editor::DrawNodeProperty() {
    if (m_selectedNodeId != 0) {
        m_graph->DrawNodeProperty(m_selectedNodeId, m_draw.get());
    }
}

void Editor::DrawNewNodeMenu(float x, float y) {
    std::string_view newNodeTypeName;
    if (ImGui::BeginMenu("All")) {
        for(const auto* it = m_graph->TypeClassesBegin(); it != m_graph->TypeClassesEnd(); ++it) {
            if (ImGui::MenuItem(it->GetPrettyName().c_str())) {
                newNodeTypeName = it->GetName();
            }
        }
        ImGui::EndMenu();
    }
    if (!newNodeTypeName.empty()) {
        auto nodeId =  m_graph->AddNode(newNodeTypeName);
        ne::SetNodePosition(ne::NodeId(nodeId), ImVec2(x, y));
    }
}

}
