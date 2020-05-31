#include "middleware/gscheme/graph/gs_graph.h"

#include <limits>
#include <cstring>

#include "core/common/exception.h"


namespace gs {

static_assert(sizeof(Pin) == 16, "sizeof(Pin) == 16 bytes");
static_assert(sizeof(Node) == 24, "sizeof(Node) == 24 bytes");
static_assert(sizeof(Graph) == 16, "sizeof(Graph) == 24 bytes");

static constexpr const uint16_t MAX_PINS_COUNT = std::numeric_limits<uint8_t>::max();
static constexpr const uint16_t MAX_NODES_COUNT = std::numeric_limits<uint16_t>::max() - 1;

void Node::SetInputPinData(uint8_t index, void* data) {
    if (index >= m_countInputPins) {
        if (m_countInputPins != 0) {
            throw EngineError("gs::Node::SetInputPinData: wrong index = {}, max value = {}", index, m_countInputPins - 1);
        }

        throw EngineError("gs::Node::SetInputPinData: wrong index = {}, no input pins", index);
    }

    m_pins[index].data = data;
}

void Node::SetOutputPinData(uint8_t index, void* data) {
    if (index >= m_countOutputPins) {
        if (m_countOutputPins != 0) {
            throw EngineError("gs::Node::SetOutputPinData: wrong index = {}, max value = {}", index, m_countOutputPins - 1);
        }

        throw EngineError("gs::Node::SetOutputPinData: wrong index = {}, no output pins", index);
    }

    m_pins[m_countInputPins + index].data = data;
}

void Node::Init(uint16_t id) {
    m_id = id;
}

void Node::Create(uint8_t countInputPins, uint8_t countOutputPins, void* data) {
    m_data = data;

    m_countInputPins = countInputPins;
    m_countOutputPins = countOutputPins;
    m_pins = new Pin[countInputPins + countOutputPins];
    uint32_t baseID = static_cast<uint32_t>(m_id) << uint32_t(16);

    uint32_t isInput = 1;
    for(uint8_t i=0; i!=m_countInputPins; ++i) {
        m_pins[i].id = baseID | (static_cast<uint32_t>(i) << uint32_t(8)) | isInput;
    }

    isInput = 0;
    for(uint8_t i=m_countInputPins; i!=(m_countInputPins + m_countOutputPins); ++i) {
        m_pins[i].id = baseID | (static_cast<uint32_t>(i) << uint32_t(8)) | isInput;
    }
}

void Node::Reset() {
    m_data = nullptr;
    if (m_pins != nullptr) {
        delete[] m_pins;
    }
}

void Node::ResetOrder() noexcept {
    m_order = 0;
}

uint16_t Node::GetOrderNumber(Node* nodes) noexcept {
    if (m_order == 0) {
        for(uint8_t i=0; i!=m_countInputPins; ++i) {
            if (m_pins[i].attachedPinID != 0) {
                uint16_t nodeId = NodeIdFromPinId(m_pins[i].attachedPinID);
                uint16_t order = nodes[nodeId - 1].GetOrderNumber(nodes) + 1;
                if (order > m_order) {
                    m_order = order;
                }
            }
        }
    }

    return m_order;
}

void Node::AttachToInputPin(uint8_t inputPinIndex, uint32_t attachedPinID) {
    m_pins[inputPinIndex].attachedPinID = attachedPinID;
}

void Node::DetachFromInputPin(uint8_t inputPinIndex) {
    m_pins[inputPinIndex].attachedPinID = 0;
}

void Node::IncLinkForOutputPin(uint8_t outputPinIndex) {
    m_pins[m_countInputPins + outputPinIndex].linksCount++;
}

void Node::DecLinkForOutputPin(uint8_t outputPinIndex) {
    m_pins[m_countInputPins + outputPinIndex].linksCount--;
}

Graph::Graph(uint16_t initialNodeCount)
    : m_free(initialNodeCount)
    , m_capacity(initialNodeCount)
    , m_firstFreeIndex(0)
    , m_nodes(new Node[m_capacity]) {

    for (uint16_t i=0; i!=m_capacity; ++i) {
        m_nodes[i].Init(i + 1);
        m_nodes[i].m_nextFreeIndex = i + 1;
    }
}

Graph::~Graph() {
    if (m_nodes != nullptr) {
        for (uint16_t i=0; i!=m_capacity; ++i) {
            m_nodes[i].Reset();
        }
        delete[] m_nodes;
    }
}

Node& Graph::AddNode(uint8_t countInputPins, uint8_t countOutputPins, void* data) {
    if (countInputPins > (MAX_PINS_COUNT - countOutputPins)) {
        throw EngineError(
            "gs::Graph::AddNode: wrong countInputPins = {} + countOutputPins = {}, max sum = {}",
            countInputPins, countOutputPins, MAX_PINS_COUNT);
    }
    if (m_free == 0) {
        if (m_capacity == MAX_NODES_COUNT) {
            throw EngineError("gs::Graph::AddNode: failed to add node, node limit exceeded");
        }

        auto* prevNodes = m_nodes;
        auto prevCapacity = m_capacity;

        if (prevCapacity < (MAX_NODES_COUNT - prevCapacity)) {
            m_capacity = prevCapacity * 2;
        } else {
            m_capacity = MAX_NODES_COUNT;
        }

        m_free = m_capacity - prevCapacity;
        m_firstFreeIndex = prevCapacity;

        m_nodes = new Node[m_capacity];
        std::memcpy(m_nodes, prevNodes, prevCapacity * sizeof(Node));
        delete[] prevNodes;
        for (uint16_t i=prevCapacity; i!=m_capacity; ++i) {
            m_nodes[i].Init(i + 1);
            m_nodes[i].m_nextFreeIndex = i + 1;
        }
    }

    m_firstFreeIndex = m_nodes[m_firstFreeIndex].m_nextFreeIndex;
    m_nodes[m_firstFreeIndex].Create(countInputPins, countOutputPins, data);
    --m_free;

    SortNodesByDependency();

    return m_nodes[m_firstFreeIndex];
}

bool Graph::TestRemoveNode(uint16_t nodeId) const noexcept {
    try {
        CheckRemoveNode(nodeId);
        return true;
    } catch(const std::exception&) {
        return false;
    }
}

void Graph::RemoveNode(uint16_t nodeId) {
    CheckRemoveNode(nodeId);

    uint16_t index = nodeId - 1;
    Node& node = m_nodes[index];

    for(uint8_t i=0; i!=node.m_countInputPins; ++i) {
        auto attachedPinID = node.m_pins[i].attachedPinID;
        if (attachedPinID != 0) {
            node.DetachFromInputPin(i);
            m_nodes[NodeIndexFromPinId(attachedPinID)].DecLinkForOutputPin(PinIndexFromPinId(attachedPinID));
        }
    }
    // TODO: remove connect for output pins

    if (m_free == 0) {
        m_firstFreeIndex = index;
        node.Reset();
    } else if (m_firstFreeIndex > index) {
        node.Reset();
        node.m_nextFreeIndex = m_firstFreeIndex;
        m_firstFreeIndex = index;
    } else {
        uint16_t lastIt = 0;
        uint16_t it = m_firstFreeIndex;
        for(;it <= index; it = m_nodes[it].m_nextFreeIndex) {
            if (it == index) {
                // double remove
                return;
            }
            lastIt = it;
        }
        m_nodes[lastIt].m_nextFreeIndex = index;
        node.m_nextFreeIndex = it;
        node.Reset();
    }

    ++m_free;

    SortNodesByDependency();
}

bool Graph::TestAddLink(uint32_t srcPinId, uint32_t dstPinId) const noexcept {
    try {
        CheckAddLink(srcPinId, dstPinId);
        return true;
    } catch(const std::exception&) {
        return false;
    }
}

uint64_t Graph::AddLink(uint32_t srcPinId, uint32_t dstPinId) {
    CheckAddLink(srcPinId, dstPinId);

    uint16_t srcNodeIndex = NodeIndexFromPinId(srcPinId);
    uint16_t dstNodeIndex = NodeIndexFromPinId(dstPinId);

    uint8_t srcPinIndex = PinIndexFromPinId(srcPinId);
    uint8_t dstPinIndex = PinIndexFromPinId(dstPinId);

    m_nodes[dstNodeIndex].AttachToInputPin(dstPinIndex, srcPinId);
    m_nodes[srcNodeIndex].IncLinkForOutputPin(srcPinIndex);

    SortNodesByDependency();

    return (static_cast<uint64_t>(srcPinId) << uint64_t(32)) | static_cast<uint64_t>(dstPinId);
}

bool Graph::TestRemoveLink(uint64_t linkId) const noexcept {
    try {
        CheckRemoveLink(linkId);
        return true;
    } catch(const std::exception&) {
        return false;
    }
}

void Graph::RemoveLink(uint64_t linkId) {
    CheckRemoveLink(linkId);

    uint32_t srcPinId = SrcPinIdFromLinkID(linkId);
    uint32_t dstPinId = DstPinIdFromLinkID(linkId);

    uint16_t srcNodeIndex = NodeIndexFromPinId(srcPinId);
    uint16_t dstNodeIndex = NodeIndexFromPinId(dstPinId);

    uint8_t srcPinIndex = PinIndexFromPinId(srcPinId);
    uint8_t dstPinIndex = PinIndexFromPinId(dstPinId);

    m_nodes[dstNodeIndex].DetachFromInputPin(dstPinIndex);
    m_nodes[srcNodeIndex].DecLinkForOutputPin(srcPinIndex);

    SortNodesByDependency();
}

void Graph::SortNodesByDependency() {
    for (uint16_t i=0; i!=m_capacity; ++i) {
        m_nodes[i].ResetOrder();
    }

    for (uint16_t i=0; i!=m_capacity; ++i) {
        if (!m_nodes[i].IsRemoved()) {
            m_nodes[i].GetOrderNumber(m_nodes);
        }
    }
}

void Graph::CheckRemoveNode(uint16_t nodeId) const {
    if (nodeId == 0) {
        throw EngineError("gs::Graph::RemoveNode: wrong nodeId = {}, min value = 1", nodeId);
    }
    if (nodeId > m_capacity) {
        throw EngineError("gs::Graph::RemoveNode: wrong nodeId = {}, max value = {}", nodeId, m_capacity);
    }
    if (m_nodes[nodeId].IsRemoved()) {
        throw EngineError("gs::Graph::RemoveNode: wrong nodeId = {}, node is removed", nodeId);
    }
}

void Graph::CheckAddLink(uint32_t srcPinId, uint32_t dstPinId) const {
    if (srcPinId == 0) {
        throw EngineError("gs::Graph::AddLink: wrong srcPinId = {}, min value = 1", srcPinId);
    }
    if (dstPinId == 0) {
        throw EngineError("gs::Graph::AddLink: wrong dstPinId = {}, min value = 1", dstPinId);
    }
    if (srcPinId == dstPinId) {
        throw EngineError("gs::Graph::AddLink: wrong srcPinId = {} and dstPinId = {}, it cannot be equivalent", srcPinId, dstPinId);
    }


    if (IsInputFromPinId(srcPinId)) {
        throw EngineError("gs::Graph::AddLink: wrong srcPinId = {}, it can be output pin", srcPinId);
    }
    if (!IsInputFromPinId(dstPinId)) {
        throw EngineError("gs::Graph::AddLink: wrong dstPinId = {}, it can be input pin", dstPinId);
    }


    uint16_t srcNodeId = NodeIdFromPinId(srcPinId);
    if (srcNodeId == 0) {
        throw EngineError("gs::Graph::AddLink: wrong srcNodeId = {} (from srcPinId = {}), min value = 1", srcNodeId, srcPinId);
    }
    if (srcNodeId > m_capacity) {
        throw EngineError("gs::Graph::AddLink: wrong srcNodeId = {} (from srcPinId = {}), max value = {}", srcNodeId, srcPinId, m_capacity);
    }
    uint16_t srcNodeIndex = srcNodeId - 1;
    if (m_nodes[srcNodeIndex].IsRemoved()) {
        throw EngineError("gs::Graph::AddLink: wrong srcNodeId = {} (from srcPinId = {}), node is removed", srcNodeId, srcPinId);
    }

    uint16_t dstNodeId = NodeIdFromPinId(dstPinId);
    if (dstNodeId == 0) {
        throw EngineError("gs::Graph::AddLink: wrong dstNodeId = {} (from dstPinId = {}), min value = 1", dstNodeId, dstPinId);
    }
    if (dstNodeId > m_capacity) {
        throw EngineError("gs::Graph::AddLink: wrong dstNodeId = {} (from dstPinId = {}), max value = {}", dstNodeId, dstPinId, m_capacity);
    }
    uint16_t dstNodeIndex = dstNodeId - 1;
    if (m_nodes[dstNodeIndex].IsRemoved()) {
        throw EngineError("gs::Graph::RemoveLink: wrong dstNodeId = {} (from dstPinId = {}), node is removed", dstNodeId, dstPinId);
    }

    if (srcNodeId == dstNodeId) {
        throw EngineError(
            "gs::Graph::AddLink: wrong srcPinId = {} and dstPinId = {} (from srcPinId = {} and dstPinId = {}), node ids cannot be equivalent",
            srcPinId, dstPinId, srcNodeId, dstNodeId);
    }


    uint8_t srcPinIndex = PinIndexFromPinId(srcPinId);
    uint8_t countSrcNodeOutputPins = m_nodes[srcNodeIndex].m_countOutputPins;
    if (srcPinIndex >= countSrcNodeOutputPins) {
        if (countSrcNodeOutputPins != 0) {
            throw EngineError(
                "gs::Graph::AddLink: wrong srcPinIndex = {} (from srcPinId = {}), max value = {}",
                srcPinIndex, srcPinId, countSrcNodeOutputPins - 1);
        }

        throw EngineError("gs::Graph::AddLink: wrong srcPinIndex = {} (from srcPinId = {}), no output pins", srcPinIndex, srcPinId);
    }

    uint8_t dstPinIndex = PinIndexFromPinId(dstPinId);
    uint8_t countDstNodeInputPins = m_nodes[dstNodeIndex].m_countInputPins;
    if (dstPinIndex >= countDstNodeInputPins) {
        if (countDstNodeInputPins != 0) {
            throw EngineError("gs::Graph::AddLink: wrong dstPinIndex = {} (from dstPinId = {}), max value = {}",
            dstPinIndex, dstPinId, countDstNodeInputPins - 1);
        }

        throw EngineError("gs::Graph::AddLink: wrong dstPinIndex = {} (from dstPinId = {}), no input pins", dstPinIndex, dstPinId);
    }
}

void Graph::CheckRemoveLink(uint64_t linkId) const {
    if (linkId == 0) {
        throw EngineError("gs::Graph::RemoveLink: wrong linkId = {}, min value = 1", linkId);
    }


    uint32_t srcPinId = SrcPinIdFromLinkID(linkId);
    uint32_t dstPinId = DstPinIdFromLinkID(linkId);

    if (srcPinId == 0) {
        throw EngineError("gs::Graph::RemoveLink: wrong srcPinId = {} (from linkId = {}), min value = 1", srcPinId, linkId);
    }
    if (dstPinId == 0) {
        throw EngineError("gs::Graph::RemoveLink: wrong dstPinId = {} (from linkId = {}), min value = 1", dstPinId, linkId);
    }
    if (srcPinId == dstPinId) {
        throw EngineError(
            "gs::Graph::RemoveLink: wrong srcPinId = {} and dstPinId = {} (from linkId = {}), it cannot be equivalent",
            srcPinId, dstPinId, linkId);
    }


    if (IsInputFromPinId(srcPinId)) {
        throw EngineError("gs::Graph::RemoveLink: wrong srcPinId = {} (from linkId = {}), it can be output pin", srcPinId, linkId);
    }
    if (!IsInputFromPinId(dstPinId)) {
        throw EngineError("gs::Graph::RemoveLink: wrong dstPinId = {} (from linkId = {}), it can be input pin", dstPinId, linkId);
    }


    uint16_t srcNodeId = NodeIdFromPinId(srcPinId);
    if (srcNodeId == 0) {
        throw EngineError("gs::Graph::RemoveLink: wrong srcNodeId = {} (from linkId = {}), min value = 1", srcNodeId, linkId);
    }
    if (srcNodeId > m_capacity) {
        throw EngineError("gs::Graph::RemoveLink: wrong srcNodeId = {} (from linkId = {}), max value = {}", srcNodeId, linkId, m_capacity);
    }
    uint16_t srcNodeIndex = srcNodeId - 1;
    if (m_nodes[srcNodeIndex].IsRemoved()) {
        throw EngineError("gs::Graph::RemoveLink: wrong srcNodeId = {} (from linkId = {}), node is removed", srcNodeId, linkId);
    }

    uint16_t dstNodeId = NodeIdFromPinId(dstPinId);
    if (dstNodeId == 0) {
        throw EngineError("gs::Graph::RemoveLink: wrong dstNodeId = {} (from linkId = {}), min value = 1", dstNodeId, linkId);
    }
    if (dstNodeId > m_capacity) {
        throw EngineError("gs::Graph::RemoveLink: wrong dstNodeId = {} (from linkId = {}), max value = {}", dstNodeId, linkId, m_capacity);
    }
    uint16_t dstNodeIndex = dstNodeId - 1;
    if (m_nodes[dstNodeIndex].IsRemoved()) {
        throw EngineError("gs::Graph::RemoveLink: wrong dstNodeId = {} (from linkId = {}), node is removed", dstNodeId, linkId);
    }

    if (srcNodeId == dstNodeId) {
        throw EngineError(
            "gs::Graph::RemoveLink: wrong srcPinId = {} and dstPinId = {} (from linkId = {}), node ids cannot be equivalent",
            srcPinId, dstPinId, linkId);
    }


    uint8_t srcPinIndex = PinIndexFromPinId(srcPinId);
    uint8_t countSrcNodeOutputPins = m_nodes[srcNodeIndex].m_countOutputPins;
    if (srcPinIndex >= countSrcNodeOutputPins) {
        if (countSrcNodeOutputPins != 0) {
            throw EngineError(
                "gs::Graph::RemoveLink: wrong srcPinIndex = {} (from linkId = {}), max value = {}",
                srcPinIndex, linkId, countSrcNodeOutputPins - 1);
        }

        throw EngineError("gs::Graph::RemoveLink: wrong srcPinIndex = {} (from linkId = {}), no output pins", srcPinIndex, linkId);
    }

    uint8_t dstPinIndex = PinIndexFromPinId(dstPinId);
    uint8_t countDstNodeInputPins = m_nodes[dstNodeIndex].m_countInputPins;
    if (dstPinIndex >= countDstNodeInputPins) {
        if (countDstNodeInputPins != 0) {
            throw EngineError("gs::Graph::RemoveLink: wrong dstPinIndex = {} (from linkId = {}), max value = {}",
            dstPinIndex, linkId, countDstNodeInputPins - 1);
        }

        throw EngineError("gs::Graph::RemoveLink: wrong dstPinIndex = {} (from linkId = {}), no input pins", dstPinIndex, linkId);
    }
}

}
