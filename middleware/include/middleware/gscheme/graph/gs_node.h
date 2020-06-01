#pragma once

#include <cstdint>

#include "core/common/ctor.h"
#include "middleware/gscheme/rttr/variant.h"


namespace gs {

struct Pin : Fixed {
    uint32_t id = 0;
    union {
        // for InputPin
        uint32_t attachedPinID = 0;
        // for OutputPin
        uint32_t linksCount;
    };
};

class TypeClass;
class Node : Fixed {
    friend class Graph;
public:
    uint16_t GetNextIndex() const noexcept { return m_nextIndex; }
    bool IsRemoved() const noexcept { return (m_pins == nullptr); }

private:
    void Init(uint16_t id) noexcept;
    void Create(TypeClass* typeClass, rttr::variant&& instance);
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
    TypeClass* m_typeClass = nullptr;
    rttr::variant m_instance;
};

}
