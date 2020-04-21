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
    if (!m_previewNode || !m_previewNode->IsFull()) {
        return;
    }
    auto size = ToSize(ImGui::GetContentRegionAvail());
    size.h = size.w;

    TextureViewPtr texView;
    texView = m_previewNode->GetTexture(size)->GetDefaultView(dg::TEXTURE_VIEW_SHADER_RESOURCE);
    Image(texView.RawPtr(), size, m_isOpenGL);
}
