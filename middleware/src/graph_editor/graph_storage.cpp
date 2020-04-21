#include "middleware/graph_editor/graph_storage.h"

#include "core/dg/texture.h"
#include "core/common/exception.h"
#include "middleware/graph_editor/graph_node.h"


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

void GraphStorage::AddNode(GraphNode* node) {
    m_nodes.push_back(GraphNode::Ref(node));
}

bool GraphStorage::AddLink(const ne::PinId pinIdFirst, const ne::PinId pinIdSecond, bool checkOnly) {
    if ((!pinIdFirst) || (!pinIdSecond) || (pinIdFirst == pinIdSecond)) {
        return false;
    }

    GraphPin* pinFirst = pinIdFirst.AsPointer<GraphPin>();
    GraphPin* pinSecond = pinIdSecond.AsPointer<GraphPin>();

    if ((pinFirst->isInput == pinSecond->isInput) ||
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

        auto linkId = ne::LinkId(m_nextId++);
        m_links[linkId] = LinkInfo{ne::PinId(srcPin), ne::PinId(dstPin)};
        ne::Link(linkId, ne::PinId(srcPin), ne::PinId(dstPin));
    }

    return true;
}

bool GraphStorage::DelLink(const ne::LinkId linkId, bool checkOnly) {
    const auto it = m_links.find(linkId);
    if (it == m_links.cend()) {
        return false;
    }

    if (!checkOnly) {
        GraphPin* dstPin = it->second.dstPin.AsPointer<GraphPin>();
        dstPin->node->DetachInput(dstPin->pinNum);

        m_links.erase(it);
    }

    return true;
}

void GraphStorage::Draw() {
    auto alpha = static_cast<uint8_t>(ImGui::GetStyle().Alpha * 255.0f);
    auto texBackgroundRaw = m_texBackground.RawPtr();

    m_selectedNode = nullptr;
    auto doubleClickedNode = ne::GetDoubleClickedNode();
    for (auto& node: m_nodes) {
        node->Draw(alpha, texBackgroundRaw, m_texBackgroundWidht, m_texBackgroundheight);
        if (doubleClickedNode == ne::NodeId(node.RawPtr())) {
           m_selectedNode = node.RawPtr();
        }
    }

    for (const auto& [linkId, info] : m_links) {
        ne::Link(linkId, info.srcPin, info.dstPin);
    }
}
