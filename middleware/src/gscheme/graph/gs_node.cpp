#include "middleware/gscheme/graph/gs_node.h"

#include <utility>

#include "core/common/exception.h"
#include "middleware/gscheme/graph/gs_id.h"
#include "middleware/gscheme/graph/gs_limits.h"
#include "middleware/gscheme/graph/gs_type_class.h"
#include "middleware/gscheme/graph/gs_draw_interface.h"


namespace gs {

static_assert(sizeof(Pin) == 24, "sizeof(Pin) == 24 bytes");
static_assert(sizeof(Node) == 48, "sizeof(Node) == 48 bytes");

Node::Node(Node&& other) noexcept {
    *this = std::move(other);
}

Node& Node::operator=(Node&& other) noexcept {
    m_id = other.m_id;
    m_countEmbeddedPins = other.m_countEmbeddedPins;
    m_countInputPins = other.m_countInputPins;
    m_countOutputPins = other.m_countOutputPins;
    m_changeState = other.m_changeState;
    m_order = other.m_order;
    m_nextIndex = other.m_nextIndex;
    m_pins = other.m_pins;
    m_typeClass = other.m_typeClass;
    m_instance = std::move(other.m_instance);

    other.m_pins = nullptr;
    other.m_typeClass = nullptr;
    other.m_instance.clear();

    return *this;
}

void Node::Init(uint16_t id) noexcept {
    m_id = id;
    // next free node index
    m_nextIndex = id;
}

void Node::Create(TypeClass* typeClass, rttr::variant&& instance) {
    m_typeClass = typeClass;
    m_instance = std::move(instance);

    m_countEmbeddedPins = m_typeClass->EmbeddedPinsCount();
    m_countInputPins = m_typeClass->InputPinsCount();
    m_countOutputPins = m_typeClass->OutputPinsCount();
    m_changeState = ChangeState::NotChanged;
    m_pins = new Pin[m_countEmbeddedPins + m_countInputPins + m_countOutputPins];
    uint32_t baseID = static_cast<uint32_t>(m_id) << uint32_t(16);

    uint32_t typePin = 3; // embeded
    for(uint8_t i=EmbededPinsBeginIndex(); i!=EmbededPinsEndIndex(); ++i) {
        m_pins[i].id = baseID | (static_cast<uint32_t>(i) << uint32_t(8)) | typePin;
        m_pins[i].attachedPinID = 0;
        m_pins[i].value = m_typeClass->GetValue(i, m_instance);
    }

    typePin = 1; // input
    for(uint8_t i=InputPinsBeginIndex(); i!=InputPinsEndIndex(); ++i) {
        m_pins[i].id = baseID | (static_cast<uint32_t>(i) << uint32_t(8)) | typePin;
        m_pins[i].attachedPinID = 0;
        m_pins[i].value = m_typeClass->GetValue(i, m_instance);
    }

    typePin = 0; // output
    for(uint8_t i=OutputPinsBeginIndex(); i!=OutputPinsEndIndex(); ++i) {
        m_pins[i].id = baseID | (static_cast<uint32_t>(i) << uint32_t(8)) | typePin;
        m_pins[i].linksCount = 0;
        m_pins[i].value = m_typeClass->GetValue(i, m_instance);
    }
}

void Node::Reset(uint16_t nextIndex) {
    m_nextIndex = nextIndex;
    m_typeClass = nullptr;
    if (m_pins != nullptr) {
        delete[] m_pins;
        m_pins = nullptr;
    }
}

uint32_t Node::GetAttachedPinId(uint8_t inputPinIndex) const noexcept {
    return m_pins[inputPinIndex].attachedPinID;
}

bool Node::IsConnectedPin(uint8_t pinIndex) const noexcept {
    return (m_pins[pinIndex].linksCount != 0);
}

bool Node::IsThisPinAttached(uint8_t inputPinIndex, uint32_t attachedPinID) const noexcept {
    return (m_pins[inputPinIndex].attachedPinID == attachedPinID);
}

bool Node::IsExistsConnectedOutputPins() const noexcept {
    for(uint8_t i=OutputPinsBeginIndex(); i!=OutputPinsEndIndex(); ++i) {
        if (m_pins[i].linksCount != 0) {
            return true;
        }
    }

    return false;
}

uint32_t Node::GetEmbededPinId(uint8_t offset) const noexcept {
    if (offset >= EmbededPinsCount()) {
        return 0;
    }

    return m_pins[EmbededPinsBeginIndex() + offset].id;
}

void Node::CheckIsValidEmbededPinId(uint32_t pinId) const {
    if (pinId == 0) {
        throw EngineError("for pinId = {}, min value = 1", pinId);
    }

    if (!IsEmbededFromPinId(pinId)) {
        throw EngineError("for pinId = {}, flag IsEmbeded not set", pinId);
    }

    if (m_countEmbeddedPins == 0) {
        throw EngineError("for pinId = {}, not found embedded pins", pinId);
    }

    uint8_t pinIndex = PinIndexFromPinId(pinId);
    if (EmbededPinsBeginIndex() > pinIndex) {
        throw EngineError("for pinId = {}, pinIndex = {} less than the minimum value = {}", pinId, pinIndex, EmbededPinsBeginIndex());
    }

    if (EmbededPinsEndIndex() <= pinIndex) {
        throw EngineError("for pinId = {}, pinIndex = {} greater than maximum value = {}", pinId, pinIndex, EmbededPinsEndIndex() - 1);
    }
}

uint32_t Node::GetInputPinId(uint8_t offset) const noexcept {
    if (offset >= InputPinsCount()) {
        return 0;
    }

    return m_pins[InputPinsBeginIndex() + offset].id;
}

void Node::CheckIsValidInputPinId(uint32_t pinId) const {
    if (pinId == 0) {
        throw EngineError("for pinId = {}, min value = 1", pinId);
    }

    if (IsEmbededFromPinId(pinId)) {
        throw EngineError("for pinId = {}, flag IsEmbeded is setting", pinId);
    }

    if (!IsInputFromPinId(pinId)) {
        throw EngineError("for pinId = {}, flag IsInput not set", pinId);
    }

    if (m_countInputPins == 0) {
        throw EngineError("for pinId = {}, not found input pins", pinId);
    }

    uint8_t pinIndex = PinIndexFromPinId(pinId);
    if (InputPinsBeginIndex() > pinIndex) {
        throw EngineError("for pinId = {}, pinIndex = {} less than the minimum value = {}", pinId, pinIndex, InputPinsBeginIndex());
    }

    if (InputPinsEndIndex() <= pinIndex) {
        throw EngineError("for pinId = {}, pinIndex = {} greater than maximum value = {}", pinId, pinIndex, InputPinsEndIndex() - 1);
    }
}

uint32_t Node::GetOutputPinId(uint8_t offset) const noexcept {
    if (offset >= OutputPinsCount()) {
        return 0;
    }

    return m_pins[OutputPinsBeginIndex() + offset].id;
}

void Node::CheckIsValidOutputPinId(uint32_t pinId) const {
    if (pinId == 0) {
        throw EngineError("for pinId = {}, min value = 1", pinId);
    }

    if (IsEmbededFromPinId(pinId)) {
        throw EngineError("for pinId = {}, flag IsEmbeded is setting", pinId);
    }

    if (IsInputFromPinId(pinId)) {
        throw EngineError("for pinId = {}, flag IsInput is setting", pinId);
    }

    if (m_countOutputPins == 0) {
        throw EngineError("for pinId = {}, not found output pins", pinId);
    }

    uint8_t pinIndex = PinIndexFromPinId(pinId);
    if (OutputPinsBeginIndex() > pinIndex) {
        throw EngineError("for pinId = {}, pinIndex = {} less than the minimum value = {}", pinId, pinIndex, OutputPinsBeginIndex());
    }

    if (OutputPinsEndIndex() <= pinIndex) {
        throw EngineError("for pinId = {}, pinIndex = {} greater than maximum value = {}", pinId, pinIndex, OutputPinsEndIndex() - 1);
    }
}

void Node::ResetOrder() noexcept {
    m_order = INVALID_ORDER_VALUE;
    m_nextIndex = INVALID_NODE_INDEX;
}

uint16_t Node::GetOrderNumber(Node* nodes) noexcept {
    if (m_order == INVALID_ORDER_VALUE) {
        m_order = 0;
        for(uint8_t i=InputPinsBeginIndex(); i!=InputPinsEndIndex(); ++i) {
            if (m_pins[i].attachedPinID != 0) {
                uint16_t nodeIndex = NodeIndexFromPinId(m_pins[i].attachedPinID);
                uint16_t order = nodes[nodeIndex].GetOrderNumber(nodes) + 1;
                if (order > m_order) {
                    m_order = order;
                }
            }
        }
    }

    return m_order;
}

void Node::ResetAcyclicityChecked() noexcept {
    m_isAcyclicityChecked = false;
}

bool Node::CheckAcyclicity(Node* nodes, uint16_t dstNodeId) noexcept {
    if (m_isAcyclicityChecked) {
        return true;
    }
    m_isAcyclicityChecked = true;
    if (m_id == dstNodeId) {
        return false;
    }

    for(uint8_t i=InputPinsBeginIndex(); i!=InputPinsEndIndex(); ++i) {
        if (m_pins[i].attachedPinID != 0) {
            if (!nodes[NodeIndexFromPinId(m_pins[i].attachedPinID)].CheckAcyclicity(nodes, dstNodeId)) {
                return false;
            }
        }
    }

    return true;
}

void Node::ResetChangeState() noexcept {
    m_changeState = ChangeState::NotChanged;
}

uint16_t Node::UpdateState(Node* nodes) {
    bool isChanged = false;
    for (uint8_t inputPinIndex=InputPinsBeginIndex(); inputPinIndex!=InputPinsEndIndex(); ++inputPinIndex) {
        uint32_t attachedPinID = m_pins[inputPinIndex].attachedPinID;
        if (attachedPinID != 0) {
            uint16_t attachedNodeIndex = NodeIndexFromPinId(attachedPinID);
            if ((m_changeState == ChangeState::NeedUpdateInputs) || (nodes[attachedNodeIndex].m_changeState != ChangeState::NotChanged)) {
                isChanged = true;
                m_typeClass->SetValue(inputPinIndex, m_instance, nodes[attachedNodeIndex].GetValue(PinIndexFromPinId(attachedPinID)));
            }
        }
    }

    if (isChanged || (m_changeState == ChangeState::NeedUpdateOutputs)) {
        for (uint8_t outputPinIndex=OutputPinsBeginIndex(); outputPinIndex!=OutputPinsEndIndex(); ++outputPinIndex) {
            isChanged = true;
            m_pins[outputPinIndex].value = m_typeClass->GetValue(outputPinIndex, m_instance);
        }
    }

    m_changeState = isChanged ? ChangeState::Updated : ChangeState::NotChanged;

    return m_nextIndex;
}

void Node::DrawGraph(IDraw* drawer) {
    drawer->OnStartDrawNode(static_cast<uintptr_t>(m_id), m_typeClass->GetPrettyName());

    if (InputPinsCount() != 0) {
        drawer->OnStartDrawInputPins();
        for (uint8_t i=InputPinsBeginIndex(); i!=InputPinsEndIndex(); ++i) {
            drawer->OnDrawPin(static_cast<uintptr_t>(m_pins[i].id), true, IsConnectedPin(i), m_typeClass->GetPinPrettyName(i));
        }
        drawer->OnFinishDrawInputPins();
    }

    if (OutputPinsCount() != 0) {
        drawer->OnStartDrawOutputPins();
        for (uint8_t i=OutputPinsBeginIndex(); i!=OutputPinsEndIndex(); ++i) {
            drawer->OnDrawPin(static_cast<uintptr_t>(m_pins[i].id), false, IsConnectedPin(i), m_typeClass->GetPinPrettyName(i));
        }
        drawer->OnFinishDrawOutputPins();
    }

    drawer->OnFinishDrawNode();
}

void Node::DrawNodeProperty(IDraw* drawer) {
    drawer->OnDrawEditingHeader(m_typeClass->GetPrettyName());
    for (uint8_t i=EmbededPinsBeginIndex(); i!=EmbededPinsEndIndex(); ++i) {
        auto value = m_typeClass->GetValue(i, m_instance);
        if (drawer->OnDrawEditingPin(m_typeClass->GetPinPrettyName(i), false, m_pins[i].value, value)) {
            SetValue(i, value);
        }
    }
    for (uint8_t i=InputPinsBeginIndex(); i!=InputPinsEndIndex(); ++i) {
        auto value = m_typeClass->GetValue(i, m_instance);
        if (drawer->OnDrawEditingPin(m_typeClass->GetPinPrettyName(i), IsConnectedPin(i), m_pins[i].value, value)) {
            SetValue(i, value);
        }
    }
}

const rttr::variant& Node::GetValue(uint8_t pinIndex) const {
    return m_pins[pinIndex].value;
}

void Node::SetValue(uint8_t pinIndex, const rttr::variant& value) {
    m_changeState = ChangeState::NeedUpdateOutputs;
    m_typeClass->SetValue(pinIndex, m_instance, value);
}

void Node::AttachToInputPin(uint8_t inputPinIndex, uint32_t attachedPinID) noexcept {
    m_changeState = ChangeState::NeedUpdateInputs;
    m_pins[inputPinIndex].attachedPinID = attachedPinID;
}

void Node::DetachFromInputPin(uint8_t inputPinIndex) {
    m_changeState = ChangeState::NeedUpdateOutputs;
    // Restore default value
    m_typeClass->SetValue(inputPinIndex, m_instance, m_pins[inputPinIndex].value);
    m_pins[inputPinIndex].attachedPinID = 0;
}

void Node::DetachFromInputPinIfExists(uint16_t attachedNodeID) {
    for(uint8_t i=InputPinsBeginIndex(); i!=InputPinsEndIndex(); ++i) {
        if (NodeIdFromPinId(m_pins[i].attachedPinID) == attachedNodeID) {
            m_changeState = ChangeState::NeedUpdateOutputs;
            // Restore default value
            m_typeClass->SetValue(i, m_instance, m_pins[i].value);
            m_pins[i].attachedPinID = 0;
        }
    }
}

void Node::IncLinkForOutputPin(uint8_t outputPinIndex) noexcept {
    m_pins[outputPinIndex].linksCount++;
}

void Node::DecLinkForOutputPin(uint8_t outputPinIndex) noexcept {
    m_pins[outputPinIndex].linksCount--;
}

}
