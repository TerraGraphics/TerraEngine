#include "middleware/graph_editor/graph_editor.h"

#include "middleware/graph_editor/graph_storage.h"
#include "middleware/graph_editor/graph_node_factory.h"


namespace ne = ax::NodeEditor;

GraphEditor::GraphEditor(const std::string& name, TexturePtr& texBackground, std::unique_ptr<GraphNodeFactory>&& factory)
    : m_name(name)
    , m_context(ne::CreateEditor())
    , m_storage(new GraphStorage(texBackground))
    , m_factory(std::move(factory)) {

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
}

void GraphEditor::AddNode(GraphNode* node) {
    if (node) {
        m_storage->AddNode(node);
    }
}

void GraphEditor::Draw() {
    ne::SetCurrentEditor(m_context);
    ne::PushStyleVar(ne::StyleVar_NodeBorderWidth, 0.f);
    ne::PushStyleVar(ne::StyleVar_HoveredNodeBorderWidth, 2.f);
    ne::PushStyleVar(ne::StyleVar_SelectedNodeBorderWidth, 2.f);
    ne::Begin(m_name.c_str());

    m_storage->Draw();

    if (ne::BeginCreate()) {
        ne::PinId pinIdFirst, pinIdSecond;
        if (ne::QueryNewLink(&pinIdFirst, &pinIdSecond)) {
            bool checkOnly = true;
            if (!m_storage->AddLink(pinIdFirst, pinIdSecond, checkOnly)) {
                ne::RejectNewItem();
            } else if (ne::AcceptNewItem()) {
                checkOnly = false;
                m_storage->AddLink(pinIdFirst, pinIdSecond, checkOnly);
            }
        }
        ne::EndCreate();
    }

    if (ne::BeginDelete()) {
        ne::LinkId linkId;
        while (ne::QueryDeletedLink(&linkId)) {
            bool checkOnly = true;
            if (!m_storage->DelLink(linkId, checkOnly)) {
                ne::RejectDeletedItem();
            } else if (ne::AcceptDeletedItem()) {
                checkOnly = false;
                m_storage->DelLink(linkId, checkOnly);
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
        if (node) {
            m_storage->AddNode(node);
            ne::SetNodePosition(ne::NodeId(node), openPopupPosition);
        }
        ImGui::EndPopup();
    }

    ImGui::PopStyleVar();
    ne::Resume();

    ne::End();
    ne::PopStyleVar(3);
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
