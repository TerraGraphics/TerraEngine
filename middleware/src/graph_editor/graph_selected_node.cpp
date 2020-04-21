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

void SelectedNode::SetNode(INodePreview* node) {
    m_node = node;
}

void SelectedNode::ResetNode() {
    m_node.Release();
}

void SelectedNode::Draw() {
    auto nodePtr = m_node.Lock();
    if (!nodePtr || !nodePtr->IsFull()) {
        return;
    }
    auto size = ToSize(ImGui::GetContentRegionAvail());
    size.h = size.w;

    TextureViewPtr texView;
    texView = nodePtr->GetTexture(size)->GetDefaultView(dg::TEXTURE_VIEW_SHADER_RESOURCE);
    Image(texView.RawPtr(), size, m_isOpenGL);
}
