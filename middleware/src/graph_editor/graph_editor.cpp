#include "middleware/graph_editor/graph_editor.h"

#include <map>
#include <utility>
#include <type_traits>

#include "imgui/imgui.h"
#include "imgui/node_editor.h"
#include "middleware/graph_editor/graph_storage.h"
#include "middleware/graph_editor/graph_node_factory.h"
#include "middleware/graph_editor/graph_selected_node.h"


struct GraphPin;

GraphEditor::GraphEditor(const std::string& name, TexturePtr& texBackground, std::unique_ptr<GraphNodeFactory>&& factory)
    : m_name(name)
    , m_config(new ne::Config())
    , m_storage(new GraphStorage(texBackground))
    , m_factory(std::move(factory)) {

    m_config->SettingsFile = "";
    m_context = ne::CreateEditor(m_config);
    m_selectedNode = std::make_shared<SelectedNode>();
}

GraphEditor::~GraphEditor() {
    if (m_storage) {
        delete m_storage;
        m_storage = nullptr;
    }

    if (m_context) {
        ne::DestroyEditor(m_context);
        m_context = nullptr;
    }

    if (m_config) {
        delete m_config;
        m_config = nullptr;
    }
}

bool GraphEditor::AddNode(GraphNode* node) {
    return m_storage->AddNode(node);
}

void GraphEditor::Draw() {
    ne::SetCurrentEditor(m_context);
    ne::PushStyleVar(ne::StyleVar_NodeBorderWidth, 0.f);
    ne::PushStyleVar(ne::StyleVar_HoveredNodeBorderWidth, 2.f);
    ne::PushStyleVar(ne::StyleVar_SelectedNodeBorderWidth, 2.f);
    ne::Begin(m_name.c_str());

    m_storage->Draw(m_selectedNode->GetNode());
    if (auto* node = m_storage->GetSelectedNode()) {
        m_selectedNode->SetNode(node, m_factory->GetPreview(node));
    } else if (ne::IsBackgroundClicked()) {
        m_selectedNode->ResetNode();
    }

    if (ne::BeginCreate()) {
        ne::PinId pinIdFirst, pinIdSecond;
        if (ne::QueryNewLink(&pinIdFirst, &pinIdSecond)) {
            bool checkOnly = true;
            auto* pinFirst = pinIdFirst.AsPointer<GraphPin>();
            auto* pinSecond = pinIdSecond.AsPointer<GraphPin>();
            if (!m_storage->AddLink(pinFirst, pinSecond, checkOnly)) {
                ne::RejectNewItem();
            } else if (ne::AcceptNewItem()) {
                checkOnly = false;
                m_storage->AddLink(pinFirst, pinSecond, checkOnly);
            }
        }
        ne::EndCreate();
    }

    if (ne::BeginDelete()) {
        ne::LinkId linkId = 0;
        while (ne::QueryDeletedLink(&linkId)) {
            bool checkOnly = true;
            if (!m_storage->DelLink(linkId.Get(), checkOnly)) {
                ne::RejectDeletedItem();
            } else if (ne::AcceptDeletedItem()) {
                checkOnly = false;
                m_storage->DelLink(linkId.Get(), checkOnly);
            }
        }

        ne::NodeId nodeId = 0;
        while (ne::QueryDeletedNode(&nodeId)) {
            bool checkOnly = true;
            auto node = nodeId.AsPointer<GraphNode>();
            if (!m_storage->DelNode(node, checkOnly)) {
                ne::RejectDeletedItem();
            } else if (ne::AcceptDeletedItem()) {
                if (m_selectedNode->GetNode() == node) {
                    m_selectedNode->ResetNode();
                }
                checkOnly = false;
                m_storage->DelNode(node, checkOnly);
            }
        }
        ne::EndDelete();
    }

    auto currentCursorPosition = ImGui::GetMousePos();
    static auto openPopupPosition = ImGui::GetMousePos();
    static ne::NodeId menuNode = 0;
    ne::Suspend();
    if (ne::ShowNodeContextMenu(&menuNode)) {
        ImGui::OpenPopup("Node Context Menu");
    } else if (ne::ShowBackgroundContextMenu()) {
        ImGui::OpenPopup("Create New Node");
        openPopupPosition = currentCursorPosition;
    }
    ne::Resume();

    ne::Suspend();
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));

    if (ImGui::BeginPopup("Create New Node")) {
        auto* node = EditorMenu();
        if (m_storage->AddNode(node)) {
            ne::SetNodePosition(ne::NodeId(node), openPopupPosition);
        }
        ImGui::EndPopup();
    }

    ImGui::PopStyleVar();
    ne::Resume();

    ne::End();
    ne::PopStyleVar(3);
}

std::function<void ()> GraphEditor::DrawProperty() {
    return [node = m_selectedNode]() {
        node->Draw();
    };
}

GraphNode* GraphEditor::EditorMenu() {
    GraphNode* node = nullptr;

    if (ImGui::BeginMenu("All")) {
        for(const auto& [name, nodeCtor]: m_factory->GetNodes()) {
           if (ImGui::MenuItem(name.c_str())) {
               node = nodeCtor();
           }
        }
        ImGui::EndMenu();
    }

    return node;
}
