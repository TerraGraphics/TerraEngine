#include "middleware/graph_editor/graph_storage.h"

#include <vector>
#include <utility>
#include <type_traits>

#include "core/dg/texture.h"
#include "core/common/exception.h"
#include "core/dg/graphics_types.h"
#include "middleware/imgui/imgui.h"
#include "middleware/imgui/imgui_node_editor.h"


namespace ne = ax::NodeEditor;

GraphStorage::GraphStorage(TexturePtr& texBackground)
    : m_texBackground(texBackground->GetDefaultView(dg::TEXTURE_VIEW_SHADER_RESOURCE)) {
    m_texBackgroundWidht = static_cast<float>(texBackground->GetDesc().Width);
    m_texBackgroundheight = static_cast<float>(texBackground->GetDesc().Height);
}

GraphStorage::~GraphStorage() {
    m_nodes.clear();
    m_links.clear();
}

bool GraphStorage::AddNode(GraphNode* node) {
    if (!node) {
        return false;
    }

    if (m_nodes.find(node) != m_nodes.end()) {
        return false;
    }
    m_nodes[node] = GraphNode::Ref(node);

    return true;
}

bool GraphStorage::DelNode(GraphNode* node, bool checkOnly) {
    if (!node) {
        return false;
    }
    auto it = m_nodes.find(node);
    if (it == m_nodes.end()) {
        return false;
    }

    std::vector<uintptr_t> delLinks;
    for (auto& [linkId, linkInfo] : m_links) {
        if ((linkInfo.dstPin->node == node) || (linkInfo.srcPin->node == node)) {
            delLinks.push_back(linkId);
        }
    }

    for (auto& linkId : delLinks) {
        if (!DelLink(linkId, checkOnly)) {
            return false;
        }
    }


    if (!checkOnly) {
        m_nodes.erase(it);
    }

    return true;
}

bool GraphStorage::AddLink(GraphPin* pinFirst, GraphPin* pinSecond, bool checkOnly) {
    if ((!pinFirst) ||
        (!pinSecond) ||
        (pinFirst == pinSecond) ||
        (pinFirst->isInput == pinSecond->isInput) ||
        (pinFirst->node == pinSecond->node) ||
        (pinFirst->node == nullptr) ||
        (pinSecond->node == nullptr) ||
        (pinFirst->pinType != pinSecond->pinType)) {
        return false;
    }

    auto* srcPin = pinFirst->isInput ? pinSecond : pinFirst;
    auto* dstPin = pinFirst->isInput ? pinFirst : pinSecond;

    if (dstPin->isConnected) {
        return false;
    }

    if (!checkOnly) {
        if (!dstPin->node->AttachInput(dstPin->pinNum, srcPin->node)) {
            throw EngineError("unexpected node attach error");
        }

        auto linkId = m_nextId++;
        m_links[linkId] = LinkInfo{srcPin, dstPin};
        ne::Link(ne::LinkId(linkId), ne::PinId(srcPin), ne::PinId(dstPin));
    }

    return true;
}

bool GraphStorage::DelLink(uintptr_t linkId, bool checkOnly) {
    const auto it = m_links.find(linkId);
    if (it == m_links.cend()) {
        return false;
    }

    if (!checkOnly) {
        GraphPin* dstPin = it->second.dstPin;
        dstPin->node->DetachInput(dstPin->pinNum);

        m_links.erase(it);
    }

    return true;
}

void GraphStorage::Draw(GraphNode* previewNode) {
    auto alpha = static_cast<uint8_t>(ImGui::GetStyle().Alpha * 255.0f);
    auto texBackgroundRaw = m_texBackground.RawPtr();

    m_selectedNode = nullptr;
    auto* doubleClickedNode = ne::GetDoubleClickedNode().AsPointer<GraphNode>();
    for (auto& [nodeRaw, _]: m_nodes) {
        nodeRaw->Draw((previewNode == nodeRaw), alpha, texBackgroundRaw, m_texBackgroundWidht, m_texBackgroundheight);
        if (doubleClickedNode == nodeRaw) {
           m_selectedNode = nodeRaw;
        }
    }

    for (const auto& [linkId, info] : m_links) {
        ne::Link(ne::LinkId(linkId), ne::PinId(info.srcPin), ne::PinId(info.dstPin));
    }
}
