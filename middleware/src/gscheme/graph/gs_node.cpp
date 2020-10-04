#include "middleware/gscheme/graph/gs_node.h"

#include <utility>

#include "core/common/exception.h"
#include "middleware/gscheme/graph/gs_id.h"
#include "middleware/gscheme/graph/gs_class.h"
#include "middleware/gscheme/graph/gs_limits.h"
#include "middleware/gscheme/graph/gs_draw_interface.h"


namespace gs {

static_assert(sizeof(Pin) == 28, "sizeof(Pin) == 28 bytes");
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
    m_class = other.m_class;
    m_instance = other.m_instance;
    m_instanceType = other.m_instanceType;

    other.m_pins = nullptr;
    other.m_class = nullptr;
    other.m_instance = nullptr;
    other.m_instanceType = nullptr;

    return *this;
}

void Node::Init(uint16_t id) noexcept {
    m_id = id;
    // next free node index
    m_nextIndex = id;
}

void Node::Create(Class* cls) {
    m_class = cls;
    cls->NewInstance(m_instance, m_instanceType);

    m_countEmbeddedPins = m_class->EmbeddedPinsCount();
    m_countInputPins = m_class->InputPinsCount();
    m_countOutputPins = m_class->OutputPinsCount();
    m_changeState = ChangeState::NotChanged;
    m_pins = new Pin[m_countEmbeddedPins + m_countInputPins + m_countOutputPins];
    uint32_t baseID = static_cast<uint32_t>(m_id) << uint32_t(16);

    uint32_t typePin = 3; // embeded
    for(uint8_t i=EmbeddedPinsBeginIndex(); i!=EmbeddedPinsEndIndex(); ++i) {
        m_pins[i].id = baseID | (static_cast<uint32_t>(i) << uint32_t(8)) | typePin;
        m_pins[i].attachedPinID = 0;
    }

    typePin = 1; // input
    for(uint8_t i=InputPinsBeginIndex(); i!=InputPinsEndIndex(); ++i) {
        m_pins[i].id = baseID | (static_cast<uint32_t>(i) << uint32_t(8)) | typePin;
        m_pins[i].attachedPinID = 0;
    }

    typePin = 0; // output
    for(uint8_t i=OutputPinsBeginIndex(); i!=OutputPinsEndIndex(); ++i) {
        m_pins[i].id = baseID | (static_cast<uint32_t>(i) << uint32_t(8)) | typePin;
        m_pins[i].linksCount = 0;
        m_pins[i].cachedValue = m_class->GetValue(i, m_instance);
    }
}

void Node::Reset(uint16_t nextIndex) {
    m_nextIndex = nextIndex;

    if (m_instance != nullptr) {
        m_class->DeleteInstance(m_instance, m_instanceType);
        m_instance = nullptr;
        m_instanceType = nullptr;
    }

    m_class = nullptr;
    if (m_pins != nullptr) {
        delete[] m_pins;
        m_pins = nullptr;
    }
}

uint32_t Node::GetEmbeddedPinId(uint8_t offset) const noexcept {
    if (offset >= EmbeddedPinsCount()) {
        return 0;
    }

    return m_pins[EmbeddedPinsBeginIndex() + offset].id;
}

void Node::CheckIsValidEmbeddedPinId(uint32_t pinId) const {
    if (pinId == 0) {
        throw EngineError("for pinId = {}, min value = 1", pinId);
    }

    if (!IsEmbeddedFromPinId(pinId)) {
        throw EngineError("for pinId = {}, flag IsEmbedded not set", pinId);
    }

    if (m_countEmbeddedPins == 0) {
        throw EngineError("for pinId = {}, not found embedded pins", pinId);
    }

    uint8_t pinIndex = PinIndexFromPinId(pinId);
    if (EmbeddedPinsBeginIndex() > pinIndex) {
        throw EngineError("for pinId = {}, pinIndex = {} less than the minimum value = {}", pinId, pinIndex, EmbeddedPinsBeginIndex());
    }

    if (EmbeddedPinsEndIndex() <= pinIndex) {
        throw EngineError("for pinId = {}, pinIndex = {} greater than maximum value = {}", pinId, pinIndex, EmbeddedPinsEndIndex() - 1);
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

    if (IsEmbeddedFromPinId(pinId)) {
        throw EngineError("for pinId = {}, flag IsEmbedded is setting", pinId);
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

    if (IsEmbeddedFromPinId(pinId)) {
        throw EngineError("for pinId = {}, flag IsEmbedded is setting", pinId);
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
                m_class->SetValue(inputPinIndex, m_instance, nodes[attachedNodeIndex].GetValue(PinIndexFromPinId(attachedPinID)));
            }
        }
    }

    if (isChanged || (m_changeState == ChangeState::NeedUpdateOutputs)) {
        for (uint8_t outputPinIndex=OutputPinsBeginIndex(); outputPinIndex!=OutputPinsEndIndex(); ++outputPinIndex) {
            isChanged = true;
            m_pins[outputPinIndex].cachedValue = m_class->GetValue(outputPinIndex, m_instance);
        }
    }

    m_changeState = isChanged ? ChangeState::Updated : ChangeState::NotChanged;

    return m_nextIndex;
}

const cpgf::GVariant& Node::GetValue(uint8_t pinIndex) const {
    return m_pins[pinIndex].cachedValue;
}

void Node::SetValue(uint8_t pinIndex, const cpgf::GVariant& value) {
    m_changeState = ChangeState::NeedUpdateOutputs;
    m_class->SetValue(pinIndex, m_instance, value);
}

void Node::ResetToDefault(uint8_t pinIndex) {
    m_changeState = ChangeState::NeedUpdateOutputs;
    m_class->ResetToDefault(pinIndex, m_instance);
}

void Node::AttachToInputPin(uint8_t inputPinIndex, uint32_t attachedPinID) noexcept {
    m_changeState = ChangeState::NeedUpdateInputs;
    m_pins[inputPinIndex].attachedPinID = attachedPinID;
}

void Node::DetachFromInputPin(uint8_t inputPinIndex) {
    ResetToDefault(inputPinIndex);
    m_pins[inputPinIndex].attachedPinID = 0;
}

void Node::DetachFromInputPinIfExists(uint16_t attachedNodeID) {
    for(uint8_t i=InputPinsBeginIndex(); i!=InputPinsEndIndex(); ++i) {
        if (NodeIdFromPinId(m_pins[i].attachedPinID) == attachedNodeID) {
            ResetToDefault(i);
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


void Node::DrawGraph(IDraw* drawer) {
    drawer->OnStartDrawNode(static_cast<uintptr_t>(m_id), m_class->GetPrettyName());

    if (InputPinsCount() != 0) {
        drawer->OnStartDrawInputPins();
        for (uint8_t i=InputPinsBeginIndex(); i!=InputPinsEndIndex(); ++i) {
            drawer->OnDrawPin(static_cast<uintptr_t>(m_pins[i].id), true, IsConnectedPin(i), m_class->GetPinPrettyName(i));
        }
        drawer->OnFinishDrawInputPins();
    }

    if (OutputPinsCount() != 0) {
        drawer->OnStartDrawOutputPins();
        for (uint8_t i=OutputPinsBeginIndex(); i!=OutputPinsEndIndex(); ++i) {
            drawer->OnDrawPin(static_cast<uintptr_t>(m_pins[i].id), false, IsConnectedPin(i), m_class->GetPinPrettyName(i));
        }
        drawer->OnFinishDrawOutputPins();
    }

    drawer->OnFinishDrawNode();
}

void Node::DrawNodeProperty(IDraw* drawer) {
    drawer->OnDrawEditingHeader(m_class->GetPrettyName());
    // all embedded and input pins
    for (uint8_t i=EmbeddedPinsBeginIndex(); i!=InputPinsEndIndex(); ++i) {
        auto value = m_class->GetValue(i, m_instance);
        auto result = drawer->OnDrawEditingPin(m_class->GetPinPrettyName(i), IsConnectedPin(i), m_class->GetPinTypeId(i), value);
        if (result == IDraw::EditResult::Changed) {
            SetValue(i, value);
        } else if (result == IDraw::EditResult::ResetToDefault) {
            ResetToDefault(i);
        }
    }
}

}
