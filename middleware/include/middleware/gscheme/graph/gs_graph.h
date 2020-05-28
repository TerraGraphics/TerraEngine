#pragma once

#include <cstdint>

/*
    valid ID (pinID, linkID, nodeID) more than 0
    uint16_t nodeID
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

struct Node {
    uint16_t id = 0;
    uint8_t countInputPins = 0;
    uint8_t countOutputPins = 0;
    Pin* pins = nullptr;
    void* data = nullptr;
};

class Graph {
    Graph(uint16_t initialNodeCount = 16);
    ~Graph();

private:
    uint16_t m_free = 0;
    uint16_t m_capacity = 0;
    Node* m_nodes = nullptr;
};

}
