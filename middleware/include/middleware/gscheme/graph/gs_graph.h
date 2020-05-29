#pragma once

#include <cstdint>

/*
    valid ID (pinID, linkID, nodeID) more than 0
    uint16_t nodeID - index in Graph::m_nodes + 1
    uint32_t pinID = nodeID << 16 | pinNumber << 8 | input / output, embedded or not
    uint64_t linkID = outputPinID << 32 | inputPinID
*/

namespace gs {

struct Pin {
    uint32_t id = 0;
    union {
        // for OutputPin
        uint32_t attachedPinID = 0;
        // for InputPin
        uint32_t linksCount;
    };
    void* data = nullptr;
};

class Node {
public:
    Node() = default;
    ~Node() = default;

    void Init(uint16_t id);
    void Clear(uint16_t nextFreeIndex);

    // return nextFreeIndex
    uint16_t Create(uint8_t countInputPins, uint8_t countOutputPins, void* data);

private:
    // nodeID - index in Graph::m_nodes + 1
    uint16_t m_id = 0;

#pragma GCC diagnostic push
#if defined(__clang__)
#pragma clang diagnostic ignored "-Wnested-anon-types"
#pragma clang diagnostic ignored "-Wgnu-anonymous-struct"
#elif defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wpedantic"
#endif
    union {
        // index in Graph::m_nodes
        uint16_t m_nextFreeIndex = 0;
        struct {
            uint8_t m_countInputPins;
            uint8_t m_countOutputPins;
        };
    };
#pragma GCC diagnostic pop

    Pin* m_pins = nullptr;
    void* m_data = nullptr;
};

class Graph {
    Graph(uint16_t initialNodeCount = 16);
    ~Graph();

    Node& AddNode(uint8_t countInputPins, uint8_t countOutputPins, void* data);

private:
    uint16_t m_free = 0;
    uint16_t m_capacity = 0;
    uint16_t m_firstFree = 0;
    Node* m_nodes = nullptr;
};

}
