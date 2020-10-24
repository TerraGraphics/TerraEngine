#include "middleware/graph_editor/graph_selected_node.h"

#include <string>
#include <cstdint>
#include <algorithm>

#include "dg/texture.h"
#include "imgui/imgui.h"
#include "core/math/types.h"
#include "dg/graphics_types.h"
#include "middleware/imgui/icon.h"
#include "middleware/imgui/widgets.h"
#include "middleware/imgui/imgui_math.h"
#include "middleware/graph_editor/graph_node.h"
#include "middleware/graph_editor/graph_node_preview.h"


SelectedNode::SelectedNode() {

}

SelectedNode::~SelectedNode() {

}

GraphNode* SelectedNode::GetNode() {
    if (m_node.IsValid()) {
        if (auto nodePtr = m_node.Lock(); nodePtr.RawPtr() != nullptr) {
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
    if (m_previewNode.RawPtr() != nullptr) {
        m_previewNode.Release();
    }
}

void SelectedNode::Draw() {
    if (auto nodePtr = m_node.Lock(); nodePtr.RawPtr() != nullptr) {
        gui::Text(nodePtr->GetName() + std::string(":"));
        nodePtr->DrawGui();
    } else {
        return;
    }

    if (!m_previewNode || !m_previewNode->IsFull()) {
        return;
    }

    gui::Text("Preview:");

    const auto size = gui::ToSizeU(ImGui::GetContentRegionAvail());
    const uint32_t borderSize = 2;
    auto minDim = std::min(size.h, size.w) - borderSize;
    minDim -= (minDim % 4);

    TextureViewPtr texView;
    texView = m_previewNode->GetTexture(math::Size(minDim, minDim))->GetDefaultView(dg::TEXTURE_VIEW_SHADER_RESOURCE);
    gui::Image(math::SizeF(static_cast<float>(minDim + borderSize), static_cast<float>(minDim + borderSize)), texView.RawPtr());
}
