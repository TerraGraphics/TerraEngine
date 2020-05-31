#include "middleware/gscheme/graph/gs_graph.h"

#include <limits>
#include <vector>
#include <cstring>

#include "core/common/exception.h"


namespace gs {

static_assert(sizeof(Pin) == 16, "sizeof(Pin) == 16 bytes");
static_assert(sizeof(Node) == 24, "sizeof(Node) == 24 bytes");
static_assert(sizeof(Graph) == 16, "sizeof(Graph) == 24 bytes");

static constexpr const uint16_t MAX_PINS_COUNT = std::numeric_limits<uint8_t>::max();
static constexpr const uint16_t MAX_NODES_COUNT = std::numeric_limits<uint16_t>::max() - 1;
static constexpr const uint16_t INVALID_NODE_INDEX = std::numeric_limits<uint16_t>::max();


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
    // next free node index
    m_nextIndex = id;
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

void Node::Reset(uint16_t nextIndex) {
    m_nextIndex = nextIndex;
    m_data = nullptr;
    if (m_pins != nullptr) {
        delete[] m_pins;
        m_pins = nullptr;
    }
}

void Node::ResetOrder() noexcept {
    m_order = 0;
    m_nextIndex = INVALID_NODE_INDEX;
}

void Node::CheckIsValidInputPinIndex(uint8_t pinIndex) const {
    if (m_countInputPins == 0) {
        throw EngineError("no input pins");
    }

    if (m_countInputPins <= pinIndex) {
        throw EngineError("max value = {}", m_countInputPins - 1);
    }
}

void Node::CheckIsValidOutputPinIndex(uint8_t pinIndex) const {
    if (m_countOutputPins == 0) {
        throw EngineError("no output pins");
    }

    if (m_countInputPins > pinIndex) {
        throw EngineError("min value = {}", m_countInputPins);
    }

    if ((m_countInputPins + m_countOutputPins) <= pinIndex) {
        throw EngineError("max value = {}", m_countInputPins + m_countOutputPins - 1);
    }
}

bool Node::IsExistsConnectedOutputPins() const noexcept {
    for(uint8_t i=m_countInputPins; i!=(m_countInputPins + m_countOutputPins); ++i) {
        if (m_pins[i].linksCount != 0) {
            return true;
        }
    }

    return false;
}

uint16_t Node::GetOrderNumber(Node* nodes) noexcept {
    if (m_order == 0) {
        m_order = 1;
        uint16_t maxAttachedNodeIndex = INVALID_NODE_INDEX;
        for(uint8_t i=0; i!=m_countInputPins; ++i) {
            if (m_pins[i].attachedPinID != 0) {
                uint16_t nodeIndex = NodeIndexFromPinId(m_pins[i].attachedPinID);
                uint16_t order = nodes[nodeIndex].GetOrderNumber(nodes) + 1;
                if (order > m_order) {
                    m_order = order;
                    maxAttachedNodeIndex = nodeIndex;
                }
            }
        }
        if (m_order > 1) {
            m_nextIndex = nodes[maxAttachedNodeIndex].SetNextCalcIndex(m_id - 1);
        }
    }

    return m_order;
}

uint16_t Node::SetNextCalcIndex(uint16_t nodeIndex) noexcept {
    uint16_t result = m_nextIndex;
    m_nextIndex = nodeIndex;

    return result;
}

void Node::ResetAcyclicityChecked() noexcept {
    m_isAcyclicityChecked = false;
}

bool Node::CheckAcyclicity(Node* nodes, uint16_t startNodeId) noexcept {
    if (m_isAcyclicityChecked) {
        return true;
    }
    m_isAcyclicityChecked = true;
    if (m_id == startNodeId) {
        return false;
    }

    for(uint8_t i=m_countInputPins; i!=(m_countInputPins + m_countOutputPins); ++i) {
        if (m_pins[i].attachedPinID != 0) {
            if (!nodes[NodeIndexFromPinId(m_pins[i].attachedPinID)].CheckAcyclicity(nodes, startNodeId)) {
                return false;
            }
        }
    }

    return true;
}

void Node::AttachToInputPin(uint8_t inputPinIndex, uint32_t attachedPinID) {
    m_pins[inputPinIndex].attachedPinID = attachedPinID;
}

void Node::DetachFromInputPin(uint8_t inputPinIndex) {
    m_pins[inputPinIndex].attachedPinID = 0;
}

void Node::DetachFromInputPinIfExists(uint16_t attachedNodeID) {
    for(uint8_t i=0; i!=m_countInputPins; ++i) {
        if (NodeIdFromPinId(m_pins[i].attachedPinID) == attachedNodeID) {
            m_pins[i].attachedPinID = 0;
        }
    }
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
    , m_firstCalcIndex(INVALID_NODE_INDEX)
    , m_nodes(new Node[m_capacity]) {

    for (uint16_t i=0; i!=m_capacity; ++i) {
        m_nodes[i].Init(i + 1);
    }
}

Graph::~Graph() {
    if (m_nodes != nullptr) {
        for (uint16_t i=0; i!=m_capacity; ++i) {
            m_nodes[i].Reset(INVALID_NODE_INDEX);
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
        }
    }

    m_firstFreeIndex = m_nodes[m_firstFreeIndex].GetNextIndex();
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

    for (auto it = node.InputPinsBegin(); it != node.InputPinsEnd(); ++it) {
        auto attachedPinID = it->attachedPinID;
        if (attachedPinID != 0) {
            m_nodes[NodeIndexFromPinId(attachedPinID)].DecLinkForOutputPin(PinIndexFromPinId(attachedPinID));
        }
    }

    for (uint16_t i=0; i!=m_capacity; ++i) {
        if ((i != index) && (!m_nodes[i].IsRemoved())) {
            m_nodes[i].DetachFromInputPinIfExists(nodeId);
        }
    }

    if (m_free == 0) {
        m_firstFreeIndex = index;
        node.Reset(INVALID_NODE_INDEX);
    } else if (m_firstFreeIndex > index) {
        node.Reset(m_firstFreeIndex);
        m_firstFreeIndex = index;
    } else {
        uint16_t lastIt = 0;
        uint16_t it = m_firstFreeIndex;
        for(;it <= index; it = m_nodes[it].GetNextIndex()) {
            if (it == index) {
                // double remove
                return;
            }
            lastIt = it;
        }
        m_nodes[lastIt].Reset(index);
        node.Reset(it);
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
}

void Graph::SortNodesByDependency() {
    for (uint16_t i=0; i!=m_capacity; ++i) {
        m_nodes[i].ResetOrder();
    }

    std::vector<uint16_t> firstIndexes;
    m_firstCalcIndex = INVALID_NODE_INDEX;
    for (uint16_t index=0; index!=m_capacity; ++index) {
        if (!m_nodes[index].IsRemoved()) {
            uint16_t order = m_nodes[index].GetOrderNumber(m_nodes);
            if (order == 1) {
                if (m_firstCalcIndex == INVALID_NODE_INDEX) {
                    m_firstCalcIndex = index;
                }
                firstIndexes.push_back(index);
            }
        }
    }

    if (firstIndexes.size() <= 1) {
        return;
    }

    uint16_t lastIndex = INVALID_NODE_INDEX;
    for (uint16_t firstIndex : firstIndexes) {
        if (lastIndex != INVALID_NODE_INDEX) {
            m_nodes[lastIndex].SetNextCalcIndex(firstIndex);
        }
        for(lastIndex = firstIndex; m_nodes[lastIndex].IsExistsConnectedOutputPins(); lastIndex = m_nodes[lastIndex].GetNextIndex()) {
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
    try {
        m_nodes[srcNodeIndex].CheckIsValidOutputPinIndex(srcPinIndex);
    } catch(const EngineError& e) {
        throw EngineError("gs::Graph::AddLink: wrong srcPinIndex = {} (from srcPinId = {}), {}", srcPinIndex, srcPinId, e.what());
    }

    uint8_t dstPinIndex = PinIndexFromPinId(dstPinId);
    try {
        m_nodes[dstNodeIndex].CheckIsValidInputPinIndex(dstPinIndex);
    } catch(const EngineError& e) {
        throw EngineError("gs::Graph::AddLink: wrong dstPinIndex = {} (from dstPinId = {}), {}", dstPinIndex, dstPinId, e.what());
    }


    for (uint16_t i=0; i!=m_capacity; ++i) {
        m_nodes[i].ResetAcyclicityChecked();
    }

    if (!m_nodes[dstNodeIndex].CheckAcyclicity(m_nodes, srcNodeId)) {
        throw EngineError(
                "gs::Graph::AddLink: wrong link from srcPinId = {} to dstPinId = {}, graph after add this link is not acyclic",
                srcPinId, dstPinId);
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
    try {
        m_nodes[srcNodeIndex].CheckIsValidOutputPinIndex(srcPinIndex);
    } catch(const EngineError& e) {
        throw EngineError("gs::Graph::RemoveLink: wrong srcPinIndex = {} (from linkId = {}), {}", srcPinIndex, linkId, e.what());
    }

    uint8_t dstPinIndex = PinIndexFromPinId(dstPinId);
    try {
        m_nodes[dstNodeIndex].CheckIsValidInputPinIndex(dstPinIndex);
    } catch(const EngineError& e) {
        throw EngineError("gs::Graph::RemoveLink: wrong dstPinIndex = {} (from linkId = {}), {}", dstPinIndex, linkId, e.what());
    }
}

}
