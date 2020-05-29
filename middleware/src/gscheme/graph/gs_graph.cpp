#include "middleware/gscheme/graph/gs_graph.h"

#include <limits>
#include <cstring>

#include "core/common/exception.h"


namespace gs {

static_assert(sizeof(Pin) == 16, "sizeof(Pin) == 16 bytes");
static_assert(sizeof(Node) == 24, "sizeof(Node) == 24 bytes");
static_assert(sizeof(Graph) == 16, "sizeof(Graph) == 24 bytes");

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

Graph::Graph(uint16_t initialNodeCount)
    : m_free(initialNodeCount)
    , m_capacity(initialNodeCount)
    , m_firstFree(0)
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
    if (m_free == 0) {
        if (m_capacity == (MAX_NODES_COUNT - 1)) {
            throw EngineError("gs:Graph:AddNode failed to add node, node limit exceeded");
        }

        auto* prevNodes = m_nodes;
        auto prevCapacity = m_capacity;

        m_capacity = MAX_NODES_COUNT;
        if (static_cast<uint32_t>(prevCapacity) * uint32_t(2) < static_cast<uint32_t>(MAX_NODES_COUNT)) {
            m_capacity = prevCapacity * 2;
        }

        m_free = m_capacity - prevCapacity;
        m_firstFree = prevCapacity;

        m_nodes = new Node[m_capacity];
        std::memcpy(m_nodes, prevNodes, prevCapacity * sizeof(Node));
        delete[] prevNodes;
        for (uint16_t i=prevCapacity; i!=m_capacity; ++i) {
            m_nodes[i].Init(i + 1);
            m_nodes[i].Clear(i + 1);
        }
    }

    m_firstFree = m_nodes[m_firstFree].Create(countInputPins, countOutputPins, data);
    --m_free;

    return m_nodes[m_firstFree];
}

}
