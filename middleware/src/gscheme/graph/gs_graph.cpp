#include "middleware/gscheme/graph/gs_graph.h"

#include <utility>
#include <algorithm>
#include <typeindex>

#include "core/common/exception.h"
#include "middleware/gscheme/graph/gs_id.h"
#include "middleware/gscheme/graph/gs_node.h"
#include "middleware/gscheme/graph/gs_limits.h"
#include "middleware/gscheme/graph/gs_class_storage.h"
#include "middleware/gscheme/graph/gs_draw_interface.h"


namespace gs {

class Class;

static_assert(sizeof(Graph) == 40, "sizeof(Graph) == 40 bytes");

Graph::Graph(const std::shared_ptr<ClassStorage>& classStorage, uint16_t initialNodeCount)
    : m_free(initialNodeCount)
    , m_capacity(initialNodeCount)
    , m_firstFreeIndex(0)
    , m_firstCalcIndex(INVALID_NODE_INDEX)
    , m_nodes(new Node[m_capacity])
    , m_indeciesForOrder(new uint16_t[m_capacity + m_capacity])
    , m_classStorage(classStorage) {

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

    if (m_indeciesForOrder != nullptr) {
        delete[] m_indeciesForOrder;
    }
    m_classStorage.reset();
}

void Graph::UpdateState() {
    for(uint16_t it = m_firstCalcIndex; it != INVALID_NODE_INDEX; it = m_nodes[it].UpdateState(m_nodes)) {
    }
    for (uint16_t i=0; i!=m_capacity; ++i) {
        m_nodes[i].ResetChangeState();
    }
}

void Graph::DrawGraph(IDraw* drawer) {
    drawer->OnStartDrawGraph();

    for (uint16_t i=0; i!=m_capacity; ++i) {
        if (!m_nodes[i].IsRemoved()) {
            m_nodes[i].DrawGraph(drawer);
        }
    }

    for (uint16_t i=0; i!=m_capacity; ++i) {
        if (!m_nodes[i].IsRemoved()) {
            for(const Pin* pin=m_nodes[i].InputPinsBegin(); pin!=m_nodes[i].InputPinsEnd(); ++pin) {
                if (pin->attachedPinID != 0) {
                    uint32_t dstPinId = pin->id;
                    uint32_t srcPinId = pin->attachedPinID;
                    drawer->OnDrawLink(
                        static_cast<uintptr_t>(LinkId(srcPinId, dstPinId)),
                        static_cast<uintptr_t>(srcPinId),
                        static_cast<uintptr_t>(dstPinId));
                }
            }
        }
    }

    drawer->OnFinishDrawGraph();
}

void Graph::DrawNodePreview(uint16_t nodeId, IDraw* drawer) {
    try {
        CheckIsValidNodeId(nodeId);
    } catch(const EngineError& e) {
        throw EngineError("gs::Graph::DrawNodePreview: wrong nodeId, {}", e.what());
    }

    m_nodes[nodeId - 1].DrawNodePreview(drawer);
}

void Graph::DrawNodeProperty(uint16_t nodeId, IDraw* drawer) {
    try {
        CheckIsValidNodeId(nodeId);
    } catch(const EngineError& e) {
        throw EngineError("gs::Graph::DrawNodeProperty: wrong nodeId, {}", e.what());
    }

    m_nodes[nodeId - 1].DrawNodeProperty(drawer);
}

const cpgf::GVariant& Graph::GetOutputValue(uint32_t pinId) const {
    try {
        CheckIsValidOutputPinId(pinId);
    } catch(const EngineError& e) {
        throw EngineError("gs::Graph::GetOutputValue: wrong pinId, {}", e.what());
    }

    return m_nodes[NodeIndexFromPinId(pinId)].GetOutputValue(PinIndexFromPinId(pinId));
}

const cpgf::GVariant& Graph::GetOutputValue(uint16_t nodeId, uint8_t outputPinOffset) const {
    try {
        CheckIsValidNodeId(nodeId);
    } catch(const EngineError& e) {
        throw EngineError("gs::Graph::GetOutputValue: wrong nodeId, {}", e.what());
    }

    return GetOutputValue(m_nodes[nodeId - 1].GetOutputPinId(outputPinOffset));
}

uint16_t Graph::AddNode(uint16_t classIndex) {
    Class* cls = m_classStorage->GetClass(classIndex);
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
        delete[] m_indeciesForOrder;
        m_indeciesForOrder = new uint16_t[m_capacity + m_capacity];
        for (uint16_t i=0; i!=prevCapacity; ++i) {
            m_nodes[i] = std::move(prevNodes[i]);
        }
        delete[] prevNodes;
        for (uint16_t i=prevCapacity; i!=m_capacity; ++i) {
            m_nodes[i].Init(i + 1);
        }
    }

    uint16_t nodeIndex = m_firstFreeIndex;
    m_firstFreeIndex = m_nodes[m_firstFreeIndex].GetNextIndex();
    m_nodes[nodeIndex].Create(cls);
    --m_free;

    SortNodesByDependency();

    return nodeIndex + 1;
}

uint16_t Graph::AddNode(std::string_view name) {
    return AddNode(m_classStorage->GetClassIndex(name));
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

bool Graph::TestAddLink(uint16_t srcNodeId, uint8_t outputPinOffset, uint16_t dstNodeId, uint8_t inputPinOffset) const noexcept {
    try {
        CheckIsValidNodeId(srcNodeId);
        CheckIsValidNodeId(dstNodeId);
    } catch(const EngineError& e) {
        return false;
    }

    uint32_t srcPinId = m_nodes[srcNodeId - 1].GetOutputPinId(outputPinOffset);
    uint32_t dstPinId = m_nodes[dstNodeId - 1].GetInputPinId(inputPinOffset);
    return TestAddLink(srcPinId, dstPinId);
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

    uint32_t attachedPinId = m_nodes[dstNodeIndex].GetAttachedPinId(dstPinIndex);
    if (attachedPinId != 0) {
        RemoveLink(LinkId(attachedPinId, dstPinId));
    }

    m_nodes[dstNodeIndex].AttachToInputPin(dstPinIndex, srcPinId, m_nodes[srcNodeIndex].GetPinType(srcPinIndex));
    m_nodes[srcNodeIndex].IncLinkForOutputPin(srcPinIndex);

    SortNodesByDependency();

    return LinkId(srcPinId, dstPinId);
}

uint64_t Graph::AddLink(uint16_t srcNodeId, uint8_t outputPinOffset, uint16_t dstNodeId, uint8_t inputPinOffset) {
    try {
        CheckIsValidNodeId(srcNodeId);
    } catch(const EngineError& e) {
        throw EngineError("gs::Graph::AddLink: wrong srcNodeId, {}", e.what());
    }
    try {
        CheckIsValidNodeId(dstNodeId);
    } catch(const EngineError& e) {
        throw EngineError("gs::Graph::AddLink: wrong dstNodeId, {}", e.what());
    }

    uint32_t srcPinId = m_nodes[srcNodeId - 1].GetOutputPinId(outputPinOffset);
    uint32_t dstPinId = m_nodes[dstNodeId - 1].GetInputPinId(inputPinOffset);
    return AddLink(srcPinId, dstPinId);
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

    uint32_t srcPinId = SrcPinIdFromLinkId(linkId);
    uint32_t dstPinId = DstPinIdFromLinkId(linkId);

    uint16_t srcNodeIndex = NodeIndexFromPinId(srcPinId);
    uint16_t dstNodeIndex = NodeIndexFromPinId(dstPinId);

    uint8_t srcPinIndex = PinIndexFromPinId(srcPinId);
    uint8_t dstPinIndex = PinIndexFromPinId(dstPinId);

    m_nodes[dstNodeIndex].DetachFromInputPin(dstPinIndex);
    m_nodes[srcNodeIndex].DecLinkForOutputPin(srcPinIndex);
}

void Graph::SetEmbeddedValueImpl(uint32_t pinId, const cpgf::GVariant& value, const std::type_info& typeInfo) {
    uint8_t pinIndex = 0;
    uint16_t nodeIndex = 0;
    auto typeIndex = std::type_index(typeInfo);
    try {
        CheckIsValidEmbeddedPinId(pinId);
        pinIndex = PinIndexFromPinId(pinId);
        nodeIndex = NodeIndexFromPinId(pinId);
        m_nodes[nodeIndex].CheckIsValidEmbeddedPinType(pinIndex, typeIndex);
    } catch(const EngineError& e) {
        throw EngineError("gs::Graph::SetEmbeddedValue: wrong pinId, {}", e.what());
    }

    m_nodes[nodeIndex].SetEmbeddedValue(pinIndex, value);
}

void Graph::SetEmbeddedValueImpl(uint16_t nodeId, uint8_t embeddedPinOffset, const cpgf::GVariant& value, const std::type_info& typeInfo) {
    try {
        CheckIsValidNodeId(nodeId);
    } catch(const EngineError& e) {
        throw EngineError("gs::Graph::SetEmbeddedValue: wrong nodeId, {}", e.what());
    }

    SetEmbeddedValueImpl(m_nodes[nodeId - 1].GetEmbeddedPinId(embeddedPinOffset), value, typeInfo);
}

void Graph::SetInputValueImpl(uint32_t pinId, const cpgf::GVariant& value, TypeId typeId) {
    try {
        CheckIsValidInputPinId(pinId);
    } catch(const EngineError& e) {
        throw EngineError("gs::Graph::SetInputValue: wrong pinId, {}", e.what());
    }

    m_nodes[NodeIndexFromPinId(pinId)].SetInputValue(PinIndexFromPinId(pinId), typeId, value);
}

void Graph::SetInputValueImpl(uint16_t nodeId, uint8_t inputPinOffset, const cpgf::GVariant& value, TypeId typeId) {
    try {
        CheckIsValidNodeId(nodeId);
    } catch(const EngineError& e) {
        throw EngineError("gs::Graph::SetInputValue: wrong nodeId, {}", e.what());
    }

    SetInputValueImpl(m_nodes[nodeId - 1].GetInputPinId(inputPinOffset), value, typeId);
}

void Graph::SortNodesByDependency() {
    std::fill(m_indeciesForOrder, m_indeciesForOrder + m_capacity + m_capacity, INVALID_NODE_INDEX);
    for (uint16_t i=0; i!=m_capacity; ++i) {
        if (!m_nodes[i].IsRemoved()) {
            m_nodes[i].ResetOrder();
        }
    }

    m_firstCalcIndex = INVALID_NODE_INDEX;
    uint16_t maxOrder = 0;
    for (uint16_t index=0; index!=m_capacity; ++index) {
        if (!m_nodes[index].IsRemoved()) {
            uint16_t order = m_nodes[index].GetOrderNumber(m_nodes);
            if ((m_firstCalcIndex == INVALID_NODE_INDEX) && (order == 0)) {
                m_firstCalcIndex = index;
            }
            if (order > maxOrder) {
                maxOrder = order;
            }

            uint16_t lastIndexForOrder = m_indeciesForOrder[order + m_capacity];
            if (lastIndexForOrder != INVALID_NODE_INDEX) {
                m_nodes[lastIndexForOrder].SetNextCalcIndex(index);
            } else {
                m_indeciesForOrder[order] = index;
            }
            m_indeciesForOrder[order + m_capacity] = index;
        }
    }

    for (uint16_t order=1; order<=maxOrder; ++order) {
        uint16_t lastIndexForPrevOrder = m_indeciesForOrder[order - 1 + m_capacity];
        uint16_t firstIndexForCurOrder = m_indeciesForOrder[order];
        m_nodes[lastIndexForPrevOrder].SetNextCalcIndex(firstIndexForCurOrder);
    }
}

void Graph::CheckIsValidNodeId(uint16_t nodeId) const {
    if (nodeId == 0) {
        throw EngineError("for nodeId = {}, min value = 1", nodeId);
    }
    if (nodeId > m_capacity) {
        throw EngineError("for nodeId = {}, max value = {}", nodeId, m_capacity);
    }
    if (m_nodes[nodeId - 1].IsRemoved()) {
        throw EngineError("for nodeId = {}, node is removed", nodeId);
    }
}

void Graph::CheckIsValidEmbeddedPinId(uint32_t pinId) const {
    uint16_t nodeId = NodeIdFromPinId(pinId);
    CheckIsValidNodeId(nodeId);
    m_nodes[nodeId - 1].CheckIsValidEmbeddedPinId(pinId);
}

void Graph::CheckIsValidInputPinId(uint32_t pinId) const {
    uint16_t nodeId = NodeIdFromPinId(pinId);
    CheckIsValidNodeId(nodeId);
    m_nodes[nodeId - 1].CheckIsValidInputPinId(pinId);
}

void Graph::CheckIsValidOutputPinId(uint32_t pinId) const {
    uint16_t nodeId = NodeIdFromPinId(pinId);
    CheckIsValidNodeId(nodeId);
    m_nodes[nodeId - 1].CheckIsValidOutputPinId(pinId);
}

void Graph::CheckRemoveNode(uint16_t nodeId) const {
    try {
        CheckIsValidNodeId(nodeId);
    } catch(const EngineError& e) {
        throw EngineError("wrong node, {}", e.what());
    }
}

void Graph::CheckAddLink(uint32_t srcPinId, uint32_t dstPinId) const {
    try {
        CheckIsValidOutputPinId(srcPinId);
    } catch(const EngineError& e) {
        throw EngineError("wrong srcPinId, {}", e.what());
    }

    try {
        CheckIsValidInputPinId(dstPinId);
    } catch(const EngineError& e) {
        throw EngineError("wrong dstPinId, {}", e.what());
    }

    if (srcPinId == dstPinId) {
        throw EngineError("wrong srcPinId = {} and dstPinId = {}, it cannot be equivalent", srcPinId, dstPinId);
    }

    const uint16_t srcNodeId = NodeIdFromPinId(srcPinId);
    const uint16_t dstNodeId = NodeIdFromPinId(dstPinId);
    if (srcNodeId == dstNodeId) {
        throw EngineError(
            "wrong srcPinId = {} and dstPinId = {}, node ids (srcNodeId = {} and dstPinId = {}) cannot be equivalent",
            srcPinId, dstPinId, srcNodeId, dstNodeId);
    }

    for (uint16_t i=0; i!=m_capacity; ++i) {
        m_nodes[i].ResetAcyclicityChecked();
    }

    if (!m_nodes[srcNodeId - 1].CheckAcyclicity(m_nodes, dstNodeId)) {
        throw EngineError(
                "wrong link from srcPinId = {} to dstPinId = {}, graph after add this link is not acyclic", srcPinId, dstPinId);
    }

    const uint8_t srcPinIndex = PinIndexFromPinId(srcPinId);
    const uint8_t dstPinIndex = PinIndexFromPinId(dstPinId);
    const uint32_t attachedPinId = m_nodes[dstNodeId - 1].GetAttachedPinId(dstPinIndex);
    if (attachedPinId == srcPinId) {
        throw EngineError(
            "wrong link from srcPinId = {} to dstPinId = {}, link already exist", srcPinId, dstPinId);
    }

    if (!m_nodes[dstNodeId - 1].CheckConvert(dstPinIndex, m_nodes[srcNodeId - 1].GetPinType(srcPinIndex))) {
        throw EngineError(
            "wrong link from srcPinId = {} to dstPinId = {}, types are not compatible", srcPinId, dstPinId);
    }

    if (attachedPinId != 0) {
        auto existsLinkId = LinkId(attachedPinId, dstPinId);
        try {
            CheckRemoveLink(existsLinkId);
        } catch(const EngineError& e) {
            throw EngineError(
                "wrong link from srcPinId = {} to dstPinId = {}, to create it, can't delete an already existing linkId = {}, {}",
                srcPinId, dstPinId, existsLinkId, e.what());
        }
    }
}

void Graph::CheckRemoveLink(uint64_t linkId) const {
    if (linkId == 0) {
        throw EngineError("wrong linkId = {}, min value = 1", linkId);
    }

    uint32_t srcPinId = SrcPinIdFromLinkId(linkId);
    try {
        CheckIsValidOutputPinId(srcPinId);
    } catch(const EngineError& e) {
        throw EngineError("wrong srcPinId (from linkId = {}), {}", linkId, e.what());
    }

    uint32_t dstPinId = DstPinIdFromLinkId(linkId);
    try {
        CheckIsValidInputPinId(dstPinId);
    } catch(const EngineError& e) {
        throw EngineError("wrong dstPinId (from linkId = {}), {}", linkId, e.what());
    }

    if (srcPinId == dstPinId) {
        throw EngineError(
            "wrong srcPinId = {} and dstPinId = {} (from linkId = {}), it cannot be equivalent", srcPinId, dstPinId, linkId);
    }

    uint16_t srcNodeId = NodeIdFromPinId(srcPinId);
    uint16_t dstNodeId = NodeIdFromPinId(dstPinId);
    if (srcNodeId == dstNodeId) {
        throw EngineError(
            "wrong srcNodeId = {} and dstNodeId = {} (from linkId = {}), node ids cannot be equivalent", srcNodeId, dstNodeId, linkId);
    }

    if (m_nodes[dstNodeId - 1].GetAttachedPinId(PinIndexFromPinId(dstPinId)) != srcPinId) {
        throw EngineError(
            "wrong linkId = {}, link does not exist", linkId);
    }
}

}
