#pragma once

#include <cstdint>

#include "core/common/ctor.h"


/*
    valid ID (pinID, linkID, nodeID) more than 0
    uint16_t nodeID - index in Graph::m_nodes + 1
    uint8_t pinIndex - index in Node::m_pins
    bool isInput - input or output pin
    bool isEmbeded - formally input pin, without the ability to connect a link to it, if this bit is set, the isInput bit is not parsed
    uint32_t pinID = nodeID << 16 | pinIndex << 8 | isEmbeded << 1 | isInput
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

inline bool IsEmbededFromPinId(uint32_t pinId) {
    return ((pinId >> uint32_t(1)) & uint32_t(1));
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
    uint16_t GetNextIndex() const noexcept { return m_nextIndex; }
    bool IsRemoved() const noexcept { return (m_pins == nullptr); }

private:
    void Init(uint16_t id) noexcept;
    void Create(uint8_t countEmbeddedPins, uint8_t countInputPins, uint8_t countOutputPins, void* data);
    void Reset(uint16_t nextIndex);

    bool IsExistsConnectedOutputPins() const noexcept;

    uint8_t EmbededPinsCount() const noexcept { return m_countEmbeddedPins; }
    uint8_t EmbededPinsBeginIndex() const noexcept { return 0; }
    uint8_t EmbededPinsEndIndex() const noexcept { return m_countEmbeddedPins; }
    const Pin* EmbededPinsBegin() const noexcept { return &m_pins[EmbededPinsBeginIndex()]; }
    const Pin* EmbededPinsEnd() const noexcept { return &m_pins[EmbededPinsEndIndex()]; }
    void CheckIsValidEmbededPinIndex(uint8_t pinIndex) const;

    uint8_t InputPinsCount() const noexcept { return m_countInputPins; }
    uint8_t InputPinsBeginIndex() const noexcept { return m_countEmbeddedPins; }
    uint8_t InputPinsEndIndex() const noexcept { return m_countEmbeddedPins + m_countInputPins; }
    const Pin* InputPinsBegin() const noexcept { return &m_pins[InputPinsBeginIndex()]; }
    const Pin* InputPinsEnd() const noexcept { return &m_pins[InputPinsEndIndex()]; }
    void CheckIsValidInputPinIndex(uint8_t pinIndex) const;

    uint8_t OutputPinsCount() const noexcept { return m_countOutputPins; }
    uint8_t OutputPinsBeginIndex() const noexcept { return m_countEmbeddedPins + m_countInputPins; }
    uint8_t OutputPinsEndIndex() const noexcept { return m_countEmbeddedPins + m_countInputPins + m_countOutputPins; }
    const Pin* OutputPinsBegin() const noexcept { return &m_pins[OutputPinsBeginIndex()]; }
    const Pin* OutputPinsEnd() const noexcept { return &m_pins[OutputPinsEndIndex()]; }
    void CheckIsValidOutputPinIndex(uint8_t pinIndex) const;

    void ResetOrder() noexcept;
    uint16_t GetOrderNumber(Node* nodes) noexcept;
    uint16_t SetNextCalcIndex(uint16_t nodeIndex) noexcept;

    void ResetAcyclicityChecked() noexcept;
    bool CheckAcyclicity(Node* nodes, uint16_t startNodeId) noexcept;

    void AttachToInputPin(uint8_t inputPinIndex, uint32_t attachedPinID) noexcept;
    void DetachFromInputPin(uint8_t inputPinIndex) noexcept;
    void DetachFromInputPinIfExists(uint16_t attachedNodeID) noexcept;
    void IncLinkForOutputPin(uint8_t outputPinIndex) noexcept;
    void DecLinkForOutputPin(uint8_t outputPinIndex) noexcept;

private:
    // nodeID - index in Graph::m_nodes + 1
    uint16_t m_id = 0;

    uint8_t m_countEmbeddedPins;
    uint8_t m_countInputPins;
    uint8_t m_countOutputPins;

    union {
        uint16_t m_order = 0;
        bool m_isAcyclicityChecked;
    };

    // index in Graph::m_nodes (next free node or next node for calc)
    uint16_t m_nextIndex;

    Pin* m_pins = nullptr;
    void* m_data = nullptr;
};

class Graph : Fixed {
public:
    Graph(uint16_t initialNodeCount = 16);
    ~Graph();

    Node& AddNode(uint8_t countEmbeddedPins, uint8_t countInputPins, uint8_t countOutputPins, void* data);

    bool TestRemoveNode(uint16_t nodeId) const noexcept;
    void RemoveNode(uint16_t nodeId);

    bool TestAddLink(uint32_t srcPinId, uint32_t dstPinId) const noexcept;
    uint64_t AddLink(uint32_t srcPinId, uint32_t dstPinId);

    bool TestRemoveLink(uint64_t linkId) const noexcept;
    void RemoveLink(uint64_t linkId);

private:
    void SortNodesByDependency();

    void CheckIsValidNodeId(uint16_t nodeId) const;
    void CheckRemoveNode(uint16_t nodeId) const;
    void CheckAddLink(uint32_t srcPinId, uint32_t dstPinId) const;
    void CheckRemoveLink(uint64_t linkId) const;

private:
    uint16_t m_free = 0;
    uint16_t m_capacity = 0;
    uint16_t m_firstFreeIndex = 0;
    uint16_t m_firstCalcIndex = 0;
    Node* m_nodes = nullptr;
};

}
