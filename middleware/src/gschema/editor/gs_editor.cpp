#include "middleware/gschema/editor/gs_editor.h"

#include <utility>
#include <string_view>

#include "imgui/imgui.h"
#include "core/math/types.h"
#include "imgui/node_editor.h"
#include "middleware/imgui/imgui_math.h"
#include "middleware/gschema/graph/gs_id.h"
#include "middleware/gschema/graph/gs_class.h"
#include "middleware/gschema/editor/gs_draw.h"
#include "middleware/gschema/graph/gs_graph.h"
#include "middleware/gschema/graph/gs_limits.h"
#include "middleware/gschema/graph/gs_class_storage.h"


namespace gs {

Editor::Editor(const std::shared_ptr<ClassStorage>& classStorage, const std::string& name, TexturePtr& texBackground)
    : m_name(name)
    , m_config(new ne::Config())
    , m_draw(new Draw(texBackground))
    , m_graph(new Graph(classStorage, 16))
    , m_classStorage(classStorage) {

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
    m_graph->AddNode("Add");
    m_graph->AddNode("Constant");
}

void Editor::DrawGraph() {
    ne::SetCurrentEditor(m_context);
    ne::Begin(m_name.c_str());

    ImGuiStyle& imStyle = ImGui::GetStyle();
    ne::Style& neStyle = ne::GetStyle();
    neStyle.NodePadding = gui::ToImGui(math::RectOffsetF(/* left */ 8, /* right */ 8, /* top */ 4, /* bottom */ 2));
    neStyle.PinRounding = imStyle.FrameRounding;
    neStyle.NodeBorderWidth = 0.f;
    neStyle.HoveredNodeBorderWidth = 2.f;
    neStyle.SelectedNodeBorderWidth = 2.f;
    neStyle.Colors[ne::StyleColor_PinRect] = imStyle.Colors[ImGuiCol_ButtonHovered];

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
                RemoveNode(nodeId);
            }
        }
        ne::EndDelete();
    }

    auto currentCursorPosition = ImGui::GetMousePos();

    ne::Suspend();

    ne::PinId menuPin = 0;
    ne::NodeId menuNode = 0;
    ne::LinkId menuLink = 0;
    bool needSavePosition = false;

    if (ne::ShowPinContextMenu(&menuPin)) {
        needSavePosition = true;
        ImGui::OpenPopup("Pin menu");
    } else if (ne::ShowNodeContextMenu(&menuNode)) {
        needSavePosition = true;
        m_menuNodeId = static_cast<uint16_t>(menuNode.Get());
        ImGui::OpenPopup("Node menu");
    } else if (ne::ShowLinkContextMenu(&menuLink)) {
        needSavePosition = true;
        ImGui::OpenPopup("Link menu");
    } else if (ne::ShowBackgroundContextMenu()) {
        needSavePosition = true;
        ImGui::OpenPopup("Background menu");
    }

    if (needSavePosition) {
        m_menuPopupX = currentCursorPosition.x;
        m_menuPopupY = currentCursorPosition.y;
    }

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
    if (ImGui::BeginPopupContextItem("Node menu")) {
        DrawNodeMenu(m_menuNodeId);
        ImGui::EndPopup();
    }

    if (ImGui::BeginPopup("Background menu")) {
        DrawNewNodeMenu(m_menuPopupX, m_menuPopupY);
        ImGui::EndPopup();
    }
    ImGui::PopStyleVar();

    ne::Resume();

    m_graph->UpdateState();

    ne::End();
}

void Editor::DrawNodePreview() {
    m_graph->DrawNodePreview(m_previewNodeId, m_draw.get());
}

void Editor::DrawNodeProperty() {
    if (m_selectedNodeId != 0) {
        m_graph->DrawNodeProperty(m_selectedNodeId, m_draw.get());
    }
}

void Editor::RemoveNode(uint16_t nodeId) {
    if (m_previewNodeId == nodeId) {
        m_previewNodeId = 0;
    }
    if (m_selectedNodeId == nodeId) {
        m_selectedNodeId = 0;
    }
    m_graph->RemoveNode(nodeId);
}

void Editor::DrawNodeMenu(uint16_t nodeId) {
    if (m_previewNodeId == nodeId) {
        if (ImGui::MenuItem("Stop previewing node")) {
            m_previewNodeId = 0;
        }
    } else {
        if (ImGui::MenuItem("Start previewing node")) {
            m_previewNodeId = nodeId;
        }
    }

    if (ImGui::MenuItem("Remove")) {
        if (m_graph->TestRemoveNode(nodeId)) {
            RemoveNode(nodeId);
        }
    }
}

void Editor::DrawNewNodeMenu(float x, float y) {
    uint16_t newNodeClassIndex = INVALID_CLASS_INDEX;
    if (ImGui::BeginMenu("All")) {
        uint16_t index = 0;
        for(const auto* it = m_classStorage->ClassesBegin(); it != m_classStorage->ClassesEnd(); ++it, ++index) {
            if (ImGui::MenuItem(it->GetDisplayName().data())) {
                newNodeClassIndex = index;
            }

        }
        ImGui::EndMenu();
    }
    if (newNodeClassIndex != INVALID_CLASS_INDEX) {
        auto nodeId =  m_graph->AddNode(newNodeClassIndex);
        ne::SetNodePosition(ne::NodeId(nodeId), ImVec2(x, y));
    }
}

}
