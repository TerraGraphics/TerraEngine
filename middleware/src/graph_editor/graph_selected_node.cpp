#include "middleware/graph_editor/graph_selected_node.h"

#include <imgui.h>

#include "core/dg/texture.h"
#include "middleware/imgui/widgets.h"
#include "middleware/imgui/imgui_math.h"
#include "middleware/graph_editor/graph_node_preview.h"


SelectedNode::SelectedNode(bool isOpenGL)
    : m_isOpenGL(isOpenGL) {

}

SelectedNode::~SelectedNode() {

}

GraphNode* SelectedNode::GetNode() {
    if (m_node.IsValid()) {
        if (auto nodePtr = m_node.Lock()) {
            return nodePtr.RawPtr();
        }
    }

    return nullptr;
}

void SelectedNode::SetNode(GraphNode* node, INodePreview* previewNode) {
    m_node = node;
    m_previewNode = previewNode;
}

void SelectedNode::ResetNode() {
    if (m_node.IsValid()) {
        m_node.Release();
    }
    if (m_previewNode) {
        m_previewNode.Release();
    }
}

void SelectedNode::Draw() {
    if (auto nodePtr = m_node.Lock()) {
        nodePtr->DrawGui();
    } else {
        return;
    }

    if (!m_previewNode || !m_previewNode->IsFull()) {
        return;
    }
    auto size = ToSize(ImGui::GetContentRegionAvail());
    size.h = size.w;

    TextureViewPtr texView;
    texView = m_previewNode->GetTexture(size)->GetDefaultView(dg::TEXTURE_VIEW_SHADER_RESOURCE);
    gui::Image(texView.RawPtr(), size, m_isOpenGL);
}
