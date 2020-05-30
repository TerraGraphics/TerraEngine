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

void Node::AttachToInputPin(uint8_t inputPinIndex, uint32_t attachedPinID) {
    if (inputPinIndex >= m_countInputPins) {
        if (m_countInputPins != 0) {
            throw EngineError("gs:Node:AttachToInputPin wrong inputPinIndex = {}, max index = {}", inputPinIndex, m_countInputPins - 1);
        }

        throw EngineError("gs:Node:AttachToInputPin wrong inputPinIndex = {}, no input pins", inputPinIndex);
    }

    m_pins[inputPinIndex].attachedPinID = attachedPinID;
}

void Node::IncLinkForOutputPin(uint8_t outputPinIndex) {
    if (outputPinIndex >= m_countOutputPins) {
        if (m_countOutputPins != 0) {
            throw EngineError("gs:Node:IncLinkForOutputPin wrong outputPinIndex = {}, max index = {}", outputPinIndex, m_countOutputPins - 1);
        }

        throw EngineError("gs:Node:IncLinkForOutputPin wrong outputPinIndex = {}, no output pins", outputPinIndex);
    }

    m_pins[m_countInputPins + outputPinIndex].linksCount++;
}

void Node::DecLinkForOutputPin(uint8_t outputPinIndex) {
    if (outputPinIndex >= m_countOutputPins) {
        if (m_countOutputPins != 0) {
            throw EngineError("gs:Node:DecLinkForOutputPin wrong outputPinIndex = {}, max index = {}", outputPinIndex, m_countOutputPins - 1);
        }

        throw EngineError("gs:Node:DecLinkForOutputPin wrong outputPinIndex = {}, no output pins", outputPinIndex);
    }

    m_pins[m_countInputPins + outputPinIndex].linksCount--;
}

void Node::SetInputPinData(uint8_t index, void* data) {
    if (index >= m_countInputPins) {
        if (m_countInputPins != 0) {
            throw EngineError("gs:Node:SetInputPinData wrong index = {}, max index = {}", index, m_countInputPins - 1);
        }

        throw EngineError("gs:Node:SetInputPinData wrong index = {}, no input pins", index);
    }

    m_pins[index].data = data;
}

void Node::SetOutputPinData(uint8_t index, void* data) {
    if (index >= m_countOutputPins) {
        if (m_countOutputPins != 0) {
            throw EngineError("gs:Node:SetOutputPinData wrong index = {}, max index = {}", index, m_countOutputPins - 1);
        }

        throw EngineError("gs:Node:SetOutputPinData wrong index = {}, no output pins", index);
    }

    m_pins[m_countInputPins + index].data = data;
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
        throw EngineError("gs:Graph:AddNode wrong value (countInputPins + countOutputPins), max sum = {}", MAX_PINS_COUNT);
    }
    if (m_free == 0) {
        if (m_capacity == MAX_NODES_COUNT) {
            throw EngineError("gs:Graph:AddNode failed to add node, node limit exceeded");
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

    return m_nodes[m_firstFreeIndex];
}

void Graph::RemoveNode(uint16_t id) {
    if (id == 0) {
        throw EngineError("gs:Graph:RemoveNode wrong id = {}, min id = 1", id);
    }
    if (id > m_capacity) {
        throw EngineError("gs:Graph:RemoveNode wrong id = {}, max id = {}", id, m_capacity);
    }
    uint16_t index = id - 1;

    if (m_free == 0) {
        m_firstFreeIndex = index;
        m_nodes[index].Reset();
    } else if (m_firstFreeIndex > index) {
        m_nodes[index].Reset();
        m_nodes[index].m_nextFreeIndex = m_firstFreeIndex;
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
        m_nodes[index].m_nextFreeIndex = it;
        m_nodes[index].Reset();
    }

    ++m_free;
}

uint64_t Graph::AddLink(uint32_t srcPinId, uint32_t dstPinId) {
    if (srcPinId == 0) {
        throw EngineError("gs:Graph:AddLink wrong srcPinId = {}, min value = 1", srcPinId);
    }
    if (dstPinId == 0) {
        throw EngineError("gs:Graph:AddLink wrong dstPinId = {}, min value = 1", dstPinId);
    }
    if (srcPinId == dstPinId) {
        throw EngineError("gs:Graph:AddLink wrong srcPinId = {} and dstPinId = {}, it cannot be equivalent", srcPinId, dstPinId);
    }


    if (IsInputFromPinId(srcPinId)) {
        throw EngineError("gs:Graph:AddLink wrong srcPinId = {}, it can be output pin", srcPinId);
    }
    if (!IsInputFromPinId(dstPinId)) {
        throw EngineError("gs:Graph:AddLink wrong dstPinId = {}, it can be input pin", dstPinId);
    }

    uint16_t srcNodeId = NodeIdFromPinId(srcPinId);
    if (srcNodeId == 0) {
        throw EngineError("gs:Graph:AddLink wrong srcPinId = {}, there is no corresponding node with id {}", srcPinId, srcNodeId);
    }
    if (srcNodeId > m_capacity) {
        throw EngineError(
            "gs:Graph:AddLink wrong srcPinId = {}, there is no corresponding node with id {}, max nodeId = {}", srcPinId, srcNodeId, m_capacity);
    }

    uint16_t dstNodeId = NodeIdFromPinId(dstPinId);
    if (dstNodeId == 0) {
        throw EngineError("gs:Graph:AddLink wrong dstPinId = {}, there is no corresponding node with id {}", dstPinId, dstNodeId);
    }
    if (dstNodeId > m_capacity) {
        throw EngineError(
            "gs:Graph:AddLink wrong dstPinId = {}, there is no corresponding node with id {}, max nodeId = {}", dstPinId, dstNodeId, m_capacity);
    }

    if (srcNodeId == dstNodeId) {
        throw EngineError(
            "gs:Graph:AddLink wrong srcPinId = {} (srcNodeId = {}) and dstPinId = {} (dstNodeId = {}), node id cannot be equivalent",
            srcPinId, srcNodeId, dstPinId, dstNodeId);
    }

    uint8_t srcPinIndex = PinIndexFromPinId(srcPinId);
    uint8_t dstPinIndex = PinIndexFromPinId(dstPinId);

    m_nodes[dstNodeId].AttachToInputPin(dstPinIndex, srcPinId);
    m_nodes[srcNodeId].IncLinkForOutputPin(srcPinIndex);

    return (static_cast<uint64_t>(srcPinId) << uint64_t(32)) | static_cast<uint64_t>(dstPinId);
}

}
