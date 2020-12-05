#pragma once

#include <string>
#include <cstdint>
#include <typeindex>

#include "cpgf/variant.h"
#include "core/common/ctor.h"
#include "middleware/gschema/graph/gs_types_decl.h"
#include "middleware/gschema/graph/gs_types_convert_func.h"


namespace gs {

struct Pin : Fixed {
    Pin() {}
    ~Pin() {}

    uint32_t id = 0;
    union {
        // for input pin
        uint32_t attachedPinID = 0;
        // for output pin
        uint32_t linksCount;
    };

    // convert attached type to pin type
    ConvertFunc convertFunc = nullptr;

    // for output pins: real type
    // for input pins: attached type
    // for embedded pins: Unknown
    TypeId typeId = TypeId::Unknown;

    // cachedValue for output pin
    cpgf::GVariant cachedValue = cpgf::GVariant();
};

class IDraw;
class Class;
class Node : Noncopyable {
    enum class ChangeState : uint8_t {
        NotChanged = 0,
        NeedUpdateInputs = 1,
        NeedUpdateOutputs = 2,
        Updated = 3,
    };

    enum ValidFlags : uint8_t {
        Valid = 0,
        ResultError = 1,
        ConvertError = 2
    };

public:
    Node() = default;
    Node(Node&& o) noexcept;
    Node& operator=(Node&& o) noexcept;

    void Init(uint16_t id) noexcept;
    void Create(Class* cls);
    void Reset(uint16_t nextIndex);

public:
    uint16_t GetNextIndex() const noexcept { return m_nextIndex; }
    bool IsRemoved() const noexcept { return (m_pins == nullptr); }

    // valid for input pins
    uint32_t GetAttachedPinId(uint8_t inputPinIndex) const noexcept { return m_pins[inputPinIndex].attachedPinID; }

    // works for all pins type
    bool IsConnectedPin(uint8_t pinIndex) const noexcept { return (m_pins[pinIndex].linksCount != 0); }

    // valid for input and putput pins type
    TypeId GetPinType(uint8_t pinIndex) const noexcept { return m_pins[pinIndex].typeId; }

    // valid for input pins
    bool CheckConvert(uint8_t inputPinIndex, TypeId attachedPinType) const;

public:
    uint32_t GetEmbeddedPinId(uint8_t offset) const noexcept;
    bool IsEmbeddedPinIndex(uint8_t index) const noexcept { return ((EmbeddedPinsBeginIndex() >= index) && (index < EmbeddedPinsEndIndex())); }
    uint8_t EmbeddedPinsCount() const noexcept { return m_countEmbeddedPins; }
    uint8_t EmbeddedPinsBeginIndex() const noexcept { return 0; }
    uint8_t EmbeddedPinsEndIndex() const noexcept { return m_countEmbeddedPins; }
    const Pin* EmbeddedPinsBegin() const noexcept { return &m_pins[EmbeddedPinsBeginIndex()]; }
    const Pin* EmbeddedPinsEnd() const noexcept { return &m_pins[EmbeddedPinsEndIndex()]; }
    void CheckIsValidEmbeddedPinId(uint32_t pinId) const;
    void CheckIsValidEmbeddedPinType(uint8_t pinIndex, std::type_index typeIndex) const;

    uint32_t GetInputPinId(uint8_t offset) const noexcept;
    bool IsInputPinIndex(uint8_t index) const noexcept { return ((InputPinsBeginIndex() >= index) && (index < InputPinsEndIndex())); }
    uint8_t InputPinsCount() const noexcept { return m_countInputPins; }
    uint8_t InputPinsBeginIndex() const noexcept { return m_countEmbeddedPins; }
    uint8_t InputPinsEndIndex() const noexcept { return m_countEmbeddedPins + m_countInputPins; }
    const Pin* InputPinsBegin() const noexcept { return &m_pins[InputPinsBeginIndex()]; }
    const Pin* InputPinsEnd() const noexcept { return &m_pins[InputPinsEndIndex()]; }
    void CheckIsValidInputPinId(uint32_t pinId) const;

    uint32_t GetOutputPinId(uint8_t offset) const noexcept;
    bool IsOutputPinIndex(uint8_t index) const noexcept { return ((OutputPinsBeginIndex() >= index) && (index < OutputPinsEndIndex())); }
    uint8_t OutputPinsCount() const noexcept { return m_countOutputPins; }
    uint8_t OutputPinsBeginIndex() const noexcept { return m_countEmbeddedPins + m_countInputPins; }
    uint8_t OutputPinsEndIndex() const noexcept { return static_cast<uint8_t>(m_countEmbeddedPins + m_countInputPins + m_countOutputPins); }
    const Pin* OutputPinsBegin() const noexcept { return &m_pins[OutputPinsBeginIndex()]; }
    const Pin* OutputPinsEnd() const noexcept { return &m_pins[OutputPinsBeginIndex()] + OutputPinsCount(); }
    void CheckIsValidOutputPinId(uint32_t pinId) const;

    uint8_t AllPinsBeginIndex() const noexcept { return 0; }
    uint8_t AllPinsEndIndex() const noexcept { return static_cast<uint8_t>(m_countEmbeddedPins + m_countInputPins + m_countOutputPins); }

public:
    void ResetOrder() noexcept;
    uint16_t GetOrderNumber(Node* nodes) noexcept;
    void SetNextCalcIndex(uint16_t nodeIndex) noexcept { m_nextIndex = nodeIndex; }

public:
    void ResetAcyclicityChecked() noexcept;
    bool CheckAcyclicity(Node* nodes, uint16_t dstNodeId) noexcept;

public:
    void ResetChangeState() noexcept;
    // return next node index for update
    uint16_t UpdateState(Node* nodes);

public:
    const cpgf::GVariant& GetOutputValue(uint8_t pinIndex) const;
    void SetInputValue(uint8_t pinIndex, TypeId typeId, const cpgf::GVariant& value);
    void SetEmbeddedValue(uint8_t pinIndex, const cpgf::GVariant& value);
    void ResetToDefault(uint8_t pinIndex);

public:
    void AttachToInputPin(uint8_t inputPinIndex, uint32_t attachedPinID, TypeId attachedPinType);
    void DetachFromInputPin(uint8_t inputPinIndex);
    void DetachFromInputPinIfExists(uint16_t attachedNodeID);
    void IncLinkForOutputPin(uint8_t outputPinIndex) noexcept;
    void DecLinkForOutputPin(uint8_t outputPinIndex) noexcept;

private:
    bool ExistsResultError() const noexcept;
    bool ExistsConvertError() const noexcept;
    void SetResultError(const std::string& msg);
    void SetConvertError();
    void RemoveResultError();
    void RemoveConvertError();

    bool NeedConvertFunc(uint8_t inputPinIndex, TypeId attachedPinType) const noexcept;
    void AttachToInputPinCalcType(uint8_t inputPinIndex, TypeId attachedPinType);
    void DetachFromInputPinCalcType(uint8_t inputPinIndex);
    void TryChange(uint8_t pinIndex);

public:
    TypeId GetValueForPreview(cpgf::GVariant& value);
    void DrawGraph(IDraw* drawer);
    void DrawNodePreview(IDraw* drawer);
    void DrawNodeProperty(IDraw* drawer);

private:
    // nodeID - index in Graph::m_nodes + 1
    uint16_t m_id = 0;

    uint8_t m_countEmbeddedPins = 0;
    uint8_t m_countInputPins = 0;
    uint8_t m_countOutputPins = 0;
    uint8_t m_outputValueVersion = 0;

    ValidFlags m_validFlags = ValidFlags::Valid;
    ChangeState m_changeState = ChangeState::NotChanged;

    union {
        uint16_t m_order = 0;
        bool m_isAcyclicityChecked;
    };

    // index in Graph::m_nodes (next free node or next node for calc)
    uint16_t m_nextIndex = 0;

    Pin* m_pins = nullptr;
    Class* m_class = nullptr;
    void* m_instance = nullptr;
    std::string m_lastResultError;
};

}
