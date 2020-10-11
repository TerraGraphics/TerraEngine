#include "middleware/gscheme/graph/gs_node.h"

#include <utility>

#include "core/common/exception.h"
#include "middleware/gscheme/graph/gs_id.h"
#include "middleware/gscheme/graph/gs_types.h"
#include "middleware/gscheme/graph/gs_class.h"
#include "middleware/gscheme/graph/gs_limits.h"
#include "middleware/gscheme/graph/gs_draw_interface.h"


namespace gs {

static_assert(sizeof(Pin) == 40, "sizeof(Pin) == 40 bytes");
static_assert(sizeof(Node) == 48, "sizeof(Node) == 48 bytes");

Node::Node(Node&& other) noexcept {
    *this = std::move(other);
}

Node& Node::operator=(Node&& other) noexcept {
    m_id = other.m_id;
    m_countEmbeddedPins = other.m_countEmbeddedPins;
    m_countInputPins = other.m_countInputPins;
    m_countOutputPins = other.m_countOutputPins;
    m_isValid = other.m_isValid;
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
    m_isValid = true;
    m_changeState = ChangeState::NotChanged;
    m_pins = new Pin[m_countEmbeddedPins + m_countInputPins + m_countOutputPins];
    constexpr const uint32_t isUniversalTypeFlag = 4;
    uint32_t baseID = static_cast<uint32_t>(m_id) << uint32_t(16);

    for(uint8_t i=AllPinsBeginIndex(); i!=AllPinsEndIndex(); ++i) {
        m_pins[i].typeId = m_class->GetDefaultPinTypeId(i);
        m_pins[i].convertFunc = nullptr;
    }

    uint32_t typePin = 3; // embeded
    for(uint8_t i=EmbeddedPinsBeginIndex(); i!=EmbeddedPinsEndIndex(); ++i) {
        m_pins[i].id = baseID | (static_cast<uint32_t>(i) << uint32_t(8)) | typePin;
        m_pins[i].attachedPinID = 0;
    }

    typePin = 1; // input
    for(uint8_t i=InputPinsBeginIndex(); i!=InputPinsEndIndex(); ++i) {
        m_pins[i].id = baseID | (static_cast<uint32_t>(i) << uint32_t(8)) | typePin;
        m_pins[i].attachedPinID = 0;
        if (HasUniversalBit(m_pins[i].typeId)) {
            m_pins[i].id |= isUniversalTypeFlag;
        }
    }

    typePin = 0; // output
    for(uint8_t i=OutputPinsBeginIndex(); i!=OutputPinsEndIndex(); ++i) {
        m_pins[i].id = baseID | (static_cast<uint32_t>(i) << uint32_t(8)) | typePin;
        m_pins[i].linksCount = 0;
        m_pins[i].cachedValue = m_class->GetValue(i, m_instance);
        if (HasUniversalBit(m_pins[i].typeId)) {
            m_pins[i].id |= isUniversalTypeFlag;
        }
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

bool Node::CheckConvert(uint8_t inputPinIndex, TypeId attachedPinType) const {
    if (IsUniversalTypeFromPinId(m_pins[inputPinIndex].id)) {
        return true;
    }

    if (m_class->GetDefaultPinTypeId(inputPinIndex) == attachedPinType) {
        return true;
    }

    return m_class->CanConvertToDefaultType(inputPinIndex, attachedPinType);
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
            if (GetAttachedPinId(i) != 0) {
                uint16_t nodeIndex = NodeIndexFromPinId(GetAttachedPinId(i));
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
        if (GetAttachedPinId(i) != 0) {
            if (!nodes[NodeIndexFromPinId(GetAttachedPinId(i))].CheckAcyclicity(nodes, dstNodeId)) {
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
        const uint32_t attachedPinId = GetAttachedPinId(inputPinIndex);
        if (attachedPinId != 0) {
            const uint16_t attachedNodeIndex = NodeIndexFromPinId(attachedPinId);
            if ((m_changeState == ChangeState::NeedUpdateInputs) || (nodes[attachedNodeIndex].m_changeState != ChangeState::NotChanged)) {
                isChanged = m_isValid;
                Pin& pin = m_pins[inputPinIndex];
                const cpgf::GVariant& value = nodes[attachedNodeIndex].GetValue(PinIndexFromPinId(attachedPinId));
                if (pin.convertFunc != nullptr) {
                    m_class->SetValue(inputPinIndex, m_instance, pin.convertFunc(value));
                } else if (m_isValid || IsUniversalTypeFromPinId(pin.id) || (m_class->GetDefaultPinTypeId(inputPinIndex) == pin.typeId)) {
                    m_class->SetValue(inputPinIndex, m_instance, value);
                }
            }
        }
    }

    if (m_isValid && (isChanged || (m_changeState == ChangeState::NeedUpdateOutputs))) {
        for (uint8_t outputPinIndex=OutputPinsBeginIndex(); outputPinIndex!=OutputPinsEndIndex(); ++outputPinIndex) {
            isChanged = true;
            m_pins[outputPinIndex].cachedValue = m_class->GetValue(outputPinIndex, m_instance);
        }
    }

    m_changeState = isChanged ? ChangeState::Updated : ChangeState::NotChanged;

    return m_nextIndex;
}

uint16_t Node::UpdateTypes(Node* nodes) {
    m_isValid = true;
    bool isExistsUniversalTypes = false;
    for(uint8_t inputPinIndex=InputPinsBeginIndex(); inputPinIndex!=InputPinsEndIndex(); ++inputPinIndex) {
        const uint32_t attachedPinID = GetAttachedPinId(inputPinIndex);
        if (attachedPinID == 0) {
            continue;
        }

        Pin& pin = m_pins[inputPinIndex];
        const TypeId typeId = GetPinType(inputPinIndex);
        const TypeId defaultTypeId = m_class->GetDefaultPinTypeId(inputPinIndex);
        const bool isUniversalPinType = IsUniversalTypeFromPinId(pin.id);
        const TypeId attachedTypeId = nodes[NodeIndexFromPinId(attachedPinID)].GetPinType(inputPinIndex);
        if (attachedTypeId == typeId) {
            if ((pin.convertFunc == nullptr) && !isUniversalPinType && (defaultTypeId != typeId)) {
                m_isValid = false;
            }

            continue;
        }

        pin.typeId = attachedTypeId;
        if (!isUniversalPinType) {
            if (defaultTypeId != attachedTypeId) {
                pin.convertFunc = m_class->GetFuncConvertToDefaultType(inputPinIndex, attachedTypeId);
                if (pin.convertFunc == nullptr) {
                    m_isValid = false;
                }
            }
            continue;
        }

        isExistsUniversalTypes = true;
        pin.convertFunc = nullptr;
        m_class->SetConcreteUniversalPinType(inputPinIndex, m_instanceType, ToUniversalTypeId(attachedTypeId));
    }

    if (isExistsUniversalTypes) {
        if (!m_class->CheckIsClassTypeValid(m_instanceType)) {
            m_isValid = false;
        }

        RecalcOutputTypes();
    }

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

void Node::AttachToInputPin(uint8_t inputPinIndex, uint32_t attachedPinID, TypeId attachedPinType) {
    m_changeState = ChangeState::NeedUpdateInputs;
    m_pins[inputPinIndex].attachedPinID = attachedPinID;
    AttachToInputPinCalcType(inputPinIndex, attachedPinType);
}

void Node::DetachFromInputPin(uint8_t inputPinIndex) {
    ResetToDefault(inputPinIndex);
    m_pins[inputPinIndex].attachedPinID = 0;
    DetachFromInputPinCalcType(inputPinIndex);
}

void Node::DetachFromInputPinIfExists(uint16_t attachedNodeID) {
    for(uint8_t i=InputPinsBeginIndex(); i!=InputPinsEndIndex(); ++i) {
        if (NodeIdFromPinId(GetAttachedPinId(i)) == attachedNodeID) {
            ResetToDefault(i);
            m_pins[i].attachedPinID = 0;
            DetachFromInputPinCalcType(i);
        }
    }
}

void Node::IncLinkForOutputPin(uint8_t outputPinIndex) noexcept {
    m_pins[outputPinIndex].linksCount++;
}

void Node::DecLinkForOutputPin(uint8_t outputPinIndex) noexcept {
    m_pins[outputPinIndex].linksCount--;
}

void Node::RecalcOutputTypes() {
    if (m_isValid) {
        for(uint8_t i=OutputPinsBeginIndex(); i!=OutputPinsEndIndex(); ++i) {
            if (IsUniversalTypeFromPinId(m_pins[i].id)) {
                m_pins[i].typeId = m_class->GetConcreteUniversalPinType(i, m_instanceType);
            }
        }
    }
}

void Node::AttachToInputPinCalcType(uint8_t inputPinIndex, TypeId attachedPinType) {
    if (attachedPinType == GetPinType(inputPinIndex)) {
        return;
    }
    m_pins[inputPinIndex].typeId = attachedPinType;

    if (!IsUniversalTypeFromPinId(m_pins[inputPinIndex].id)) {
        if (m_class->GetDefaultPinTypeId(inputPinIndex) != attachedPinType) {
            m_pins[inputPinIndex].convertFunc = m_class->GetFuncConvertToDefaultType(inputPinIndex, attachedPinType);
            if (m_pins[inputPinIndex].convertFunc == nullptr) {
                m_isValid = false;
            }
        }
        return;
    }

    m_pins[inputPinIndex].convertFunc = nullptr;
    m_class->SetConcreteUniversalPinType(inputPinIndex, m_instanceType, ToUniversalTypeId(attachedPinType));
    if (!m_class->CheckIsClassTypeValid(m_instanceType)) {
        m_isValid = false;
    }

    RecalcOutputTypes();
}

void Node::DetachFromInputPinCalcType(uint8_t inputPinIndex) {
    m_pins[inputPinIndex].typeId = m_class->GetDefaultPinTypeId(inputPinIndex);
    m_pins[inputPinIndex].convertFunc = nullptr;

    if (!m_isValid) {
        m_isValid = true;
        for(uint8_t i=InputPinsBeginIndex(); i!=InputPinsEndIndex(); ++i) {
            if ((GetAttachedPinId(i) != 0) &&
                (m_pins[i].convertFunc == nullptr) &&
                !IsUniversalTypeFromPinId(m_pins[i].id) &&
                (m_class->GetDefaultPinTypeId(i) != GetPinType(i))) {
                m_isValid = false;
                break;
            }
        }
    }

    if (IsUniversalTypeFromPinId(m_pins[inputPinIndex].id)) {
        m_class->ResetUniversalPinTypeToDefault(inputPinIndex, m_instanceType);
        if (!m_class->CheckIsClassTypeValid(m_instanceType)) {
            m_isValid = false;
        }

        RecalcOutputTypes();
    }
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
