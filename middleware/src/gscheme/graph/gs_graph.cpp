#include "middleware/gscheme/graph/gs_graph.h"

#include <limits>
#include <vector>
#include <cstring>

#include "core/common/exception.h"


namespace gs {

static_assert(sizeof(Pin) == 16, "sizeof(Pin) == 16 bytes");
static_assert(sizeof(Node) == 32, "sizeof(Node) == 32 bytes");
static_assert(sizeof(Graph) == 16, "sizeof(Graph) == 24 bytes");

static constexpr const uint16_t MAX_PINS_COUNT = std::numeric_limits<uint8_t>::max() - 1;
static constexpr const uint16_t MAX_NODES_COUNT = std::numeric_limits<uint16_t>::max() - 1;
static constexpr const uint16_t INVALID_NODE_INDEX = std::numeric_limits<uint16_t>::max();


void Node::Init(uint16_t id) noexcept {
    m_id = id;
    // next free node index
    m_nextIndex = id;
}

void Node::Create(uint8_t countEmbeddedPins, uint8_t countInputPins, uint8_t countOutputPins, void* data) {
    m_data = data;

    m_countEmbeddedPins = countEmbeddedPins;
    m_countInputPins = countInputPins;
    m_countOutputPins = countOutputPins;
    m_pins = new Pin[countInputPins + countOutputPins];
    uint32_t baseID = static_cast<uint32_t>(m_id) << uint32_t(16);

    uint32_t typePin = 3; // embeded
    for(uint8_t i=EmbededPinsBeginIndex(); i!=EmbededPinsEndIndex(); ++i) {
        m_pins[i].id = baseID | (static_cast<uint32_t>(i) << uint32_t(8)) | typePin;
    }

    typePin = 1; // input
    for(uint8_t i=InputPinsBeginIndex(); i!=InputPinsEndIndex(); ++i) {
        m_pins[i].id = baseID | (static_cast<uint32_t>(i) << uint32_t(8)) | typePin;
    }

    typePin = 0; // output
    for(uint8_t i=OutputPinsBeginIndex(); i!=OutputPinsEndIndex(); ++i) {
        m_pins[i].id = baseID | (static_cast<uint32_t>(i) << uint32_t(8)) | typePin;
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

bool Node::IsExistsConnectedOutputPins() const noexcept {
    for(uint8_t i=OutputPinsBeginIndex(); i!=OutputPinsEndIndex(); ++i) {
        if (m_pins[i].linksCount != 0) {
            return true;
        }
    }

    return false;
}

void Node::CheckIsValidEmbededPinIndex(uint8_t pinIndex) const {
    if (m_countEmbeddedPins == 0) {
        throw EngineError("no embedded pins");
    }

    if (EmbededPinsBeginIndex() > pinIndex) {
        throw EngineError("min value = {}", EmbededPinsBeginIndex());
    }

    if (EmbededPinsEndIndex() <= pinIndex) {
        throw EngineError("max value = {}", EmbededPinsEndIndex() - 1);
    }
}

void Node::CheckIsValidInputPinIndex(uint8_t pinIndex) const {
    if (m_countInputPins == 0) {
        throw EngineError("no input pins");
    }

    if (InputPinsBeginIndex() > pinIndex) {
        throw EngineError("min value = {}", InputPinsBeginIndex());
    }

    if (InputPinsEndIndex() <= pinIndex) {
        throw EngineError("max value = {}", InputPinsEndIndex() - 1);
    }
}

void Node::CheckIsValidOutputPinIndex(uint8_t pinIndex) const {
    if (m_countOutputPins == 0) {
        throw EngineError("no output pins");
    }

    if (OutputPinsBeginIndex() > pinIndex) {
        throw EngineError("min value = {}", OutputPinsBeginIndex());
    }

    if (OutputPinsEndIndex() <= pinIndex) {
        throw EngineError("max value = {}", OutputPinsEndIndex() - 1);
    }
}

uint16_t Node::GetOrderNumber(Node* nodes) noexcept {
    if (m_order == 0) {
        m_order = 1;
        uint16_t maxAttachedNodeIndex = INVALID_NODE_INDEX;
        for(uint8_t i=InputPinsBeginIndex(); i!=InputPinsEndIndex(); ++i) {
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

    for(uint8_t i=InputPinsBeginIndex(); i!=InputPinsEndIndex(); ++i) {
        if (m_pins[i].attachedPinID != 0) {
            if (!nodes[NodeIndexFromPinId(m_pins[i].attachedPinID)].CheckAcyclicity(nodes, startNodeId)) {
                return false;
            }
        }
    }

    return true;
}

void Node::AttachToInputPin(uint8_t inputPinIndex, uint32_t attachedPinID) noexcept {
    m_pins[inputPinIndex].attachedPinID = attachedPinID;
}

void Node::DetachFromInputPin(uint8_t inputPinIndex) noexcept {
    m_pins[inputPinIndex].attachedPinID = 0;
}

void Node::DetachFromInputPinIfExists(uint16_t attachedNodeID) noexcept {
    for(uint8_t i=InputPinsBeginIndex(); i!=InputPinsEndIndex(); ++i) {
        if (NodeIdFromPinId(m_pins[i].attachedPinID) == attachedNodeID) {
            m_pins[i].attachedPinID = 0;
        }
    }
}

void Node::IncLinkForOutputPin(uint8_t outputPinIndex) noexcept {
    m_pins[outputPinIndex].linksCount++;
}

void Node::DecLinkForOutputPin(uint8_t outputPinIndex) noexcept {
    m_pins[outputPinIndex].linksCount--;
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

Node& Graph::AddNode(uint8_t countEmbeddedPins, uint8_t countInputPins, uint8_t countOutputPins, void* data) {
    uint16_t countPins = static_cast<uint16_t>(countEmbeddedPins) + static_cast<uint16_t>(countInputPins) + static_cast<uint16_t>(countOutputPins);
    if (countPins > static_cast<uint16_t>(MAX_PINS_COUNT)) {
        throw EngineError(
            "gs::Graph::AddNode: wrong countEmbeddedPins = {} + countInputPins = {} + countOutputPins = {}, max sum = {}",
            countEmbeddedPins, countInputPins, countOutputPins, MAX_PINS_COUNT);
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

    uint16_t nodeIndex = m_firstFreeIndex;
    m_firstFreeIndex = m_nodes[m_firstFreeIndex].GetNextIndex();
    m_nodes[nodeIndex].Create(countEmbeddedPins, countInputPins, countOutputPins, data);
    --m_free;

    SortNodesByDependency();

    return m_nodes[nodeIndex];
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
    try {
        CheckRemoveNode(nodeId);
    } catch(const EngineError& e) {
        throw EngineError("gs::Graph::RemoveNode: {}", e.what());
    }

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
    try {
        CheckAddLink(srcPinId, dstPinId);
    } catch(const EngineError& e) {
        throw EngineError("gs::Graph::AddLink: {}", e.what());
    }

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
    try {
        CheckRemoveLink(linkId);
    } catch(const EngineError& e) {
        throw EngineError("gs::Graph::RemoveLink: {}", e.what());
    }

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

void Graph::CheckIsValidNodeId(uint16_t nodeId) const {
    if (nodeId == 0) {
        throw EngineError("min value = 1");
    }
    if (nodeId > m_capacity) {
        throw EngineError("max value = {}", m_capacity);
    }
    if (m_nodes[nodeId - 1].IsRemoved()) {
        throw EngineError("node is removed");
    }
}

void Graph::CheckRemoveNode(uint16_t nodeId) const {
    try {
        CheckIsValidNodeId(nodeId);
    } catch(const EngineError& e) {
        throw EngineError("wrong nodeId = {}, {}", nodeId, e.what());
    }
}

void Graph::CheckAddLink(uint32_t srcPinId, uint32_t dstPinId) const {
    if (srcPinId == 0) {
        throw EngineError("wrong srcPinId = {}, min value = 1", srcPinId);
    }
    if (dstPinId == 0) {
        throw EngineError("wrong dstPinId = {}, min value = 1", dstPinId);
    }
    if (srcPinId == dstPinId) {
        throw EngineError("wrong srcPinId = {} and dstPinId = {}, it cannot be equivalent", srcPinId, dstPinId);
    }


    if (IsEmbededFromPinId(srcPinId) || IsInputFromPinId(srcPinId)) {
        throw EngineError("wrong srcPinId = {}, it can be input pin", srcPinId);
    }
    if (IsEmbededFromPinId(dstPinId) || (!IsInputFromPinId(dstPinId))) {
        throw EngineError("wrong dstPinId = {}, it can be input pin", dstPinId);
    }


    uint16_t srcNodeId = NodeIdFromPinId(srcPinId);
    try {
        CheckIsValidNodeId(srcNodeId);
    } catch(const EngineError& e) {
        throw EngineError("wrong srcNodeId = {} (from srcPinId = {}), {}", srcNodeId, srcPinId, e.what());
    }

    uint16_t dstNodeId = NodeIdFromPinId(dstPinId);
    try {
        CheckIsValidNodeId(dstNodeId);
    } catch(const EngineError& e) {
        throw EngineError("wrong dstNodeId = {} (from dstPinId = {}), {}", dstNodeId, dstPinId, e.what());
    }

    if (srcNodeId == dstNodeId) {
        throw EngineError(
            "wrong srcPinId = {} and dstPinId = {} (from srcPinId = {} and dstPinId = {}), node ids cannot be equivalent",
            srcPinId, dstPinId, srcNodeId, dstNodeId);
    }


    uint8_t srcPinIndex = PinIndexFromPinId(srcPinId);
    try {
        m_nodes[srcNodeId - 1].CheckIsValidOutputPinIndex(srcPinIndex);
    } catch(const EngineError& e) {
        throw EngineError("wrong srcPinIndex = {} (from srcPinId = {}), {}", srcPinIndex, srcPinId, e.what());
    }

    uint8_t dstPinIndex = PinIndexFromPinId(dstPinId);
    try {
        m_nodes[dstNodeId - 1].CheckIsValidInputPinIndex(dstPinIndex);
    } catch(const EngineError& e) {
        throw EngineError("wrong dstPinIndex = {} (from dstPinId = {}), {}", dstPinIndex, dstPinId, e.what());
    }


    for (uint16_t i=0; i!=m_capacity; ++i) {
        m_nodes[i].ResetAcyclicityChecked();
    }

    if (!m_nodes[dstNodeId - 1].CheckAcyclicity(m_nodes, srcNodeId)) {
        throw EngineError(
                "wrong link from srcPinId = {} to dstPinId = {}, graph after add this link is not acyclic", srcPinId, dstPinId);
    }
}

void Graph::CheckRemoveLink(uint64_t linkId) const {
    if (linkId == 0) {
        throw EngineError("wrong linkId = {}, min value = 1", linkId);
    }


    uint32_t srcPinId = SrcPinIdFromLinkID(linkId);
    uint32_t dstPinId = DstPinIdFromLinkID(linkId);

    if (srcPinId == 0) {
        throw EngineError("wrong srcPinId = {} (from linkId = {}), min value = 1", srcPinId, linkId);
    }
    if (dstPinId == 0) {
        throw EngineError("wrong dstPinId = {} (from linkId = {}), min value = 1", dstPinId, linkId);
    }
    if (srcPinId == dstPinId) {
        throw EngineError(
            "wrong srcPinId = {} and dstPinId = {} (from linkId = {}), it cannot be equivalent", srcPinId, dstPinId, linkId);
    }


    if (IsEmbededFromPinId(srcPinId) || IsInputFromPinId(srcPinId)) {
        throw EngineError("wrong srcPinId = {} (from linkId = {}), it can be output pin", srcPinId, linkId);
    }
    if (IsEmbededFromPinId(dstPinId) || (!IsInputFromPinId(dstPinId))) {
        throw EngineError("wrong dstPinId = {} (from linkId = {}), it can be input pin", dstPinId, linkId);
    }


    uint16_t srcNodeId = NodeIdFromPinId(srcPinId);
    try {
        CheckIsValidNodeId(srcNodeId);
    } catch(const EngineError& e) {
        throw EngineError("wrong srcNodeId = {} (from linkId = {}), {}", srcNodeId, linkId, e.what());
    }

    uint16_t dstNodeId = NodeIdFromPinId(dstPinId);
    try {
        CheckIsValidNodeId(dstNodeId);
    } catch(const EngineError& e) {
        throw EngineError("wrong dstNodeId = {} (from linkId = {}), {}", dstNodeId, linkId, e.what());
    }

    if (srcNodeId == dstNodeId) {
        throw EngineError(
            "wrong srcPinId = {} and dstPinId = {} (from linkId = {}), node ids cannot be equivalent", srcPinId, dstPinId, linkId);
    }


    uint8_t srcPinIndex = PinIndexFromPinId(srcPinId);
    try {
        m_nodes[srcNodeId - 1].CheckIsValidOutputPinIndex(srcPinIndex);
    } catch(const EngineError& e) {
        throw EngineError("wrong srcPinIndex = {} (from linkId = {}), {}", srcPinIndex, linkId, e.what());
    }

    uint8_t dstPinIndex = PinIndexFromPinId(dstPinId);
    try {
        m_nodes[dstNodeId - 1].CheckIsValidInputPinIndex(dstPinIndex);
    } catch(const EngineError& e) {
        throw EngineError("wrong dstPinIndex = {} (from linkId = {}), {}", dstPinIndex, linkId, e.what());
    }
}

}
