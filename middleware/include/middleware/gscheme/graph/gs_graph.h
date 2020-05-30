#pragma once

#include <cstdint>

/*
    valid ID (pinID, linkID, nodeID) more than 0
    uint16_t nodeID - index in Graph::m_nodes + 1
    uint8_t pinIndex - index in Node::m_pins
    bool isInput - input or output pin
    uint32_t pinID = nodeID << 16 | pinIndex << 8 | isInput
    uint64_t linkID = srcPinID << 32 | dstPinID == outputPinID << 32 | inputPinID
*/

namespace gs {

inline uint16_t NodeIdFromPinId(uint32_t pinId) {
    return static_cast<uint16_t>(pinId >> uint32_t(16));
}

inline uint8_t PinIndexFromPinId(uint32_t pinId) {
    return static_cast<uint8_t>((pinId >> uint32_t(8)) & uint32_t(0xFF));
}

inline bool IsInputFromPinId(uint32_t pinId) {
    return ((pinId & uint32_t(1)) == 1);
}

inline uint32_t SrcPinIdFromLinkID(uint64_t linkId) {
    return static_cast<uint32_t>(linkId >> uint64_t(32));
}

inline uint32_t DstPinIdFromLinkID(uint64_t linkId) {
    return static_cast<uint32_t>(linkId & uint64_t(0xFFFFFFFF));
}

struct Pin {
    uint32_t id = 0;
    union {
        // for InputPin
        uint32_t attachedPinID = 0;
        // for OutputPin
        uint32_t linksCount;
    };
    void* data = nullptr;
};

class Node {
    friend class Graph;
public:
    void SetInputPinData(uint8_t index, void* data);
    void SetOutputPinData(uint8_t index, void* data);

private:
    void Init(uint16_t id);
    void Create(uint8_t countInputPins, uint8_t countOutputPins, void* data);
    void Reset();

    void AttachToInputPin(uint8_t inputPinIndex, uint32_t attachedPinID);
    void DetachFromInputPin(uint8_t inputPinIndex);
    void IncLinkForOutputPin(uint8_t outputPinIndex);
    void DecLinkForOutputPin(uint8_t outputPinIndex);

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
    void RemoveNode(uint16_t nodeId);

    uint64_t AddLink(uint32_t srcPinId, uint32_t dstPinId);
    void RemoveLink(uint64_t linkId);

private:
    uint16_t m_free = 0;
    uint16_t m_capacity = 0;
    uint16_t m_firstFreeIndex = 0;
    Node* m_nodes = nullptr;
};

}
