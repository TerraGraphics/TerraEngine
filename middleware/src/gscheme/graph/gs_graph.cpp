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

void Node::Clear(uint16_t nextFreeIndex) {
    m_nextFreeIndex = nextFreeIndex;
    m_data = nullptr;
    if (m_pins != nullptr) {
        delete[] m_pins;
    }
}

uint16_t Node::Create(uint8_t countInputPins, uint8_t countOutputPins, void* data) {
    uint16_t nextFreeIndex = m_nextFreeIndex;
    m_countInputPins = countInputPins;
    m_countOutputPins = countOutputPins;
    m_pins = new Pin[countInputPins + countOutputPins];
    m_data = data;
    return nextFreeIndex;
}

void Node::SetInputPinData(uint8_t index, void* data) {
    if (index >= m_countInputPins) {
        if (m_countInputPins != 0) {
            throw EngineError("gs:Node:SetInputPinData wrong index, max index = {}", m_countInputPins - 1);
        }

        throw EngineError("gs:Node:SetInputPinData wrong index, no input pins");
    }

    m_pins[index].data = data;
}

void Node::SetOutputPinData(uint8_t index, void* data) {
    if (index >= m_countOutputPins) {
        if (m_countOutputPins != 0) {
            throw EngineError("gs:Node:SetOutputPinData wrong index, max index = {}", m_countOutputPins - 1);
        }

        throw EngineError("gs:Node:SetOutputPinData wrong index, no output pins");
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
        m_nodes[i].Clear(i + 1);
    }
}

Graph::~Graph() {
    if (m_nodes != nullptr) {
        for (uint16_t i=0; i!=m_capacity; ++i) {
            m_nodes[i].Clear(0);
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
            m_nodes[i].Clear(i + 1);
        }
    }

    m_firstFreeIndex = m_nodes[m_firstFreeIndex].Create(countInputPins, countOutputPins, data);
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
        m_nodes[index].Clear(0);
    } else if (m_firstFreeIndex > index) {
        m_firstFreeIndex = index;
        m_nodes[index].Clear(m_firstFreeIndex);
    } else {
        uint16_t it = m_firstFreeIndex;
        for(;it <= index; it = m_nodes[it].GetNextFreeIndex()) {
            if (it == index) {
                // double remove
                return;
            }
        }
        m_nodes[it].Clear(index);
        m_nodes[index].Clear(it);
    }

    ++m_free;
}

}
