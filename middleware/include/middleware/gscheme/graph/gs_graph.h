#pragma once

#include <cstdint>

#include "core/common/ctor.h"


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

inline uint16_t NodeIndexFromPinId(uint32_t pinId) {
    return static_cast<uint16_t>(pinId >> uint32_t(16)) - 1;
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

struct Pin : Fixed {
    uint32_t id = 0;
    union {
        // for InputPin
        uint32_t attachedPinID = 0;
        // for OutputPin
        uint32_t linksCount;
    };
    void* data = nullptr;
};

class Node : Fixed {
    friend class Graph;
public:
    void SetInputPinData(uint8_t index, void* data);
    void SetOutputPinData(uint8_t index, void* data);

private:
    bool IsRemoved() const noexcept { return (m_pins == nullptr); }

    void Init(uint16_t id);
    void Create(uint8_t countInputPins, uint8_t countOutputPins, void* data);
    void Reset(uint16_t nextIndex);

    uint16_t GetNextIndex() const noexcept { return m_nextIndex; }

    void ResetOrder() noexcept;
    uint16_t GetOrderNumber(Node* nodes) noexcept;
    uint16_t SetNextCalcIndex(uint16_t nodeIndex);

    void AttachToInputPin(uint8_t inputPinIndex, uint32_t attachedPinID);
    void DetachFromInputPin(uint8_t inputPinIndex);
    void IncLinkForOutputPin(uint8_t outputPinIndex);
    void DecLinkForOutputPin(uint8_t outputPinIndex);

private:
    // nodeID - index in Graph::m_nodes + 1
    uint16_t m_id = 0;

    uint8_t m_countInputPins;
    uint8_t m_countOutputPins;

    uint16_t m_order = 0;
    // index in Graph::m_nodes (next free node or next node for calc)
    uint16_t m_nextIndex;

    Pin* m_pins = nullptr;
    void* m_data = nullptr;
};

class Graph : Fixed {
public:
    Graph(uint16_t initialNodeCount = 16);
    ~Graph();

    Node& AddNode(uint8_t countInputPins, uint8_t countOutputPins, void* data);

    bool TestRemoveNode(uint16_t nodeId) const noexcept;
    void RemoveNode(uint16_t nodeId);

    bool TestAddLink(uint32_t srcPinId, uint32_t dstPinId) const noexcept;
    uint64_t AddLink(uint32_t srcPinId, uint32_t dstPinId);

    bool TestRemoveLink(uint64_t linkId) const noexcept;
    void RemoveLink(uint64_t linkId);

private:
    void SortNodesByDependency();
    void CheckRemoveNode(uint16_t nodeId) const;
    void CheckAddLink(uint32_t srcPinId, uint32_t dstPinId) const;
    void CheckRemoveLink(uint64_t linkId) const;

private:
    uint16_t m_free = 0;
    uint16_t m_capacity = 0;
    uint16_t m_firstFreeIndex = 0;
    Node* m_nodes = nullptr;
};

}