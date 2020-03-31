#include "middleware/graph_editor/graph_storage.h"

#include "core/common/exception.h"
#include "middleware/graph_editor/graph_node.h"


namespace ne = ax::NodeEditor;

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
        dstPin->isConnected = true;
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
        dstPin->isConnected = false;
        dstPin->node->DetachInput(dstPin->pinNum);

        m_links.erase(it);
    }

    return true;
}

void GraphStorage::Draw() {
    for (auto& node: m_nodes) {
        node->Draw();
    }

    for (const auto& [linkId, info] : m_links) {
        ne::Link(linkId, info.srcPin, info.dstPin);
    }
}
