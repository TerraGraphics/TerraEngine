#include "middleware/gschema/graph/gs_node.h"

#include <vector>
#include <utility>
#include <variant>
#include <string_view>

#include "core/common/meta.h"
#include "core/common/exception.h"
#include "middleware/gschema/graph/gs_id.h"
#include "middleware/gschema/graph/gs_types.h"
#include "middleware/gschema/graph/gs_class.h"
#include "middleware/gschema/graph/gs_limits.h"
#include "middleware/gschema/meta/gs_type_instance.h"
#include "middleware/gschema/graph/gs_draw_interface.h"


namespace gs {

static_assert(sizeof(Pin) == 40, "sizeof(Pin) == 40 bytes");
static_assert(sizeof(Node) == 72, "sizeof(Node) == 72 bytes");

Node::Node(Node&& other) noexcept {
    *this = std::move(other);
}

Node& Node::operator=(Node&& o) noexcept {
    std::swap(m_id, o.m_id);
    std::swap(m_countEmbeddedPins, o.m_countEmbeddedPins);
    std::swap(m_countInputPins, o.m_countInputPins);
    std::swap(m_countOutputPins, o.m_countOutputPins);
    std::swap(m_outputValueVersion, o.m_outputValueVersion);
    std::swap(m_validFlags, o.m_validFlags);
    std::swap(m_changeState, o.m_changeState);
    std::swap(m_order, o.m_order);
    std::swap(m_nextIndex, o.m_nextIndex);
    std::swap(m_pins, o.m_pins);
    std::swap(m_class, o.m_class);
    std::swap(m_instance, o.m_instance);
    std::swap(m_lastResultError, o.m_lastResultError);

    return *this;
}

void Node::Init(uint16_t id) noexcept {
    m_id = id;
    // next free node index
    m_nextIndex = id;
}

void Node::Create(Class* cls) {
    m_class = cls;
    m_instance = cls->NewInstance();

    m_countEmbeddedPins = m_class->EmbeddedPinsCount();
    m_countInputPins = m_class->InputPinsCount();
    m_countOutputPins = m_class->OutputPinsCount();
    m_outputValueVersion = 0;
    m_lastResultError.clear();
    m_validFlags = ValidFlags::Valid;
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
            m_pins[i].typeId = GetUniversalTypeId(cpgf::fromVariant<UniversalType>(m_pins[i].cachedValue));
        }
    }
}

void Node::Reset(uint16_t nextIndex) {
    m_nextIndex = nextIndex;

    if (m_instance != nullptr) {
        m_class->DeleteInstance(m_instance);
        m_instance = nullptr;
    }

    m_class = nullptr;
    if (m_pins != nullptr) {
        delete[] m_pins;
        m_pins = nullptr;
    }
}

bool Node::CheckConvert(uint8_t inputPinIndex, TypeId attachedPinType) const {
    if (!NeedConvertFunc(inputPinIndex, attachedPinType)) {
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

void Node::CheckIsValidEmbeddedPinType(uint8_t pinIndex, std::type_index typeIndex) const {
    auto pinTypeIndex = m_class->GetTypeIndexForEmbedded(pinIndex);
    if (pinTypeIndex != typeIndex) {
        throw EngineError("for pinIndex = {} type \"{}\" not applicable, type \"{}\" required", pinIndex,
            meta::DemangleTypeName(typeIndex.name()),
            meta::DemangleTypeName(pinTypeIndex.name()));
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
    RemoveConvertError();
    for (uint8_t inputPinIndex=InputPinsBeginIndex(); inputPinIndex!=InputPinsEndIndex(); ++inputPinIndex) {
        const uint32_t attachedPinId = GetAttachedPinId(inputPinIndex);
        if (attachedPinId == 0) {
            continue;
        }

        Pin& pin = m_pins[inputPinIndex];
        const TypeId typeId = GetPinType(inputPinIndex);
        const uint16_t attachedNodeIndex = NodeIndexFromPinId(attachedPinId);
        const TypeId attachedTypeId = nodes[attachedNodeIndex].GetPinType(PinIndexFromPinId(attachedPinId));

        if (attachedTypeId == typeId) {
            if ((pin.convertFunc == nullptr) && NeedConvertFunc(inputPinIndex, attachedTypeId)) {
                SetConvertError();
                continue;
            }
        } else {
            pin.typeId = attachedTypeId;
            if (NeedConvertFunc(inputPinIndex, attachedTypeId)) {
                pin.convertFunc = m_class->GetFuncConvertToDefaultType(inputPinIndex, attachedTypeId);
                if (pin.convertFunc == nullptr) {
                    SetConvertError();
                    continue;
                }
            }
        }

        if ((m_changeState == ChangeState::NeedUpdateInputs) || (nodes[attachedNodeIndex].m_changeState != ChangeState::NotChanged)) {
            isChanged = true;
            const cpgf::GVariant& value = nodes[attachedNodeIndex].GetOutputValue(PinIndexFromPinId(attachedPinId));
            if (pin.convertFunc != nullptr) {
                m_class->SetValue(inputPinIndex, m_instance, pin.convertFunc(value));
            } else {
                m_class->SetValue(inputPinIndex, m_instance, value);
            }
        }
    }

    if (!ExistsConvertError() && (isChanged || (m_changeState == ChangeState::NeedUpdateOutputs))) {
        for (uint8_t outputPinIndex=OutputPinsBeginIndex(); outputPinIndex!=OutputPinsEndIndex(); ++outputPinIndex) {
            try {
                auto& pin = m_pins[outputPinIndex];
                pin.cachedValue = m_class->GetValue(outputPinIndex, m_instance);
                isChanged = true;
                ++m_outputValueVersion;
                if (HasUniversalBit(pin.typeId)) {
                    pin.typeId = GetUniversalTypeId(cpgf::fromVariant<UniversalType>(pin.cachedValue));
                }
                RemoveResultError();
            } catch(const std::exception& e) {
                SetResultError(e.what());
            }
        }
    }

    m_changeState = isChanged ? ChangeState::Updated : ChangeState::NotChanged;

    return m_nextIndex;
}

const cpgf::GVariant& Node::GetOutputValue(uint8_t pinIndex) const {
    return m_pins[pinIndex].cachedValue;
}

void Node::SetInputValue(uint8_t pinIndex, TypeId typeId, const cpgf::GVariant& value) {
    if (IsConnectedPin(pinIndex)) {
        throw EngineError("gs::Node::SetValue: trying to change the connected pin (pinId = {})", m_pins[pinIndex].id);
    }

    if (ToBaseTypeId(m_pins[pinIndex].typeId) != typeId) {
        throw EngineError(
            "gs::Node::SetValue: wrong value type = {}, it is not the same as the default type = {}", typeId, m_pins[pinIndex].typeId);
    }
    if (NeedConvertFunc(pinIndex, typeId)) {
        auto convertFunc = m_class->GetFuncConvertToDefaultType(pinIndex, typeId);
        if (convertFunc == nullptr) {
            throw EngineError("gs::Node::SetValue: failed to get convert function (pinId = {}) from type {} to {}",
                m_pins[pinIndex].id, typeId, m_pins[pinIndex].typeId);
        }
        m_class->SetValue(pinIndex, m_instance, convertFunc(value));
    } else {
        m_class->SetValue(pinIndex, m_instance, value);
    }
    m_changeState = ChangeState::NeedUpdateOutputs;
}

void Node::SetEmbeddedValue(uint8_t pinIndex, const cpgf::GVariant& value) {
    m_class->SetValue(pinIndex, m_instance, value);
    m_changeState = ChangeState::NeedUpdateOutputs;
}

void Node::ResetToDefault(uint8_t pinIndex) {
    if (IsConnectedPin(pinIndex)) {
        throw EngineError("gs::Node::ResetToDefault: trying to change the connected pin (pinId = {})", m_pins[pinIndex].id);
    }

    m_class->ResetToDefault(pinIndex, m_instance);
    m_changeState = ChangeState::NeedUpdateOutputs;
}

void Node::AttachToInputPin(uint8_t inputPinIndex, uint32_t attachedPinID, TypeId attachedPinType) {
    m_changeState = ChangeState::NeedUpdateInputs;
    m_pins[inputPinIndex].attachedPinID = attachedPinID;
    AttachToInputPinCalcType(inputPinIndex, attachedPinType);
}

void Node::DetachFromInputPin(uint8_t inputPinIndex) {
    m_pins[inputPinIndex].attachedPinID = 0;
    ResetToDefault(inputPinIndex);
    DetachFromInputPinCalcType(inputPinIndex);
}

void Node::DetachFromInputPinIfExists(uint16_t attachedNodeID) {
    for(uint8_t i=InputPinsBeginIndex(); i!=InputPinsEndIndex(); ++i) {
        if (NodeIdFromPinId(GetAttachedPinId(i)) == attachedNodeID) {
            m_pins[i].attachedPinID = 0;
            ResetToDefault(i);
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

bool Node::ExistsResultError() const noexcept {
    return ((m_validFlags & ValidFlags::ResultError) != 0);
}

bool Node::ExistsConvertError() const noexcept {
    return ((m_validFlags & ValidFlags::ConvertError) != 0);
}

void Node::SetResultError(const std::string& msg) {
    m_validFlags = static_cast<ValidFlags>(m_validFlags | ValidFlags::ResultError);
    m_lastResultError = msg;
}

void Node::SetConvertError() {
    m_validFlags = static_cast<ValidFlags>(m_validFlags | ValidFlags::ConvertError);
}

void Node::RemoveResultError() {
    m_validFlags = static_cast<ValidFlags>(m_validFlags & ~ValidFlags::ResultError);
    m_lastResultError.clear();
}

void Node::RemoveConvertError() {
    m_validFlags = static_cast<ValidFlags>(m_validFlags & ~ValidFlags::ConvertError);
}

bool Node::NeedConvertFunc(uint8_t inputPinIndex, TypeId attachedPinType) const noexcept {
    TypeId defaultTypeId = m_class->GetDefaultPinTypeId(inputPinIndex);
    if (HasUniversalBit(defaultTypeId) && HasUniversalBit(attachedPinType)) {
        return false;
    }

    return (defaultTypeId != attachedPinType);
}

void Node::AttachToInputPinCalcType(uint8_t inputPinIndex, TypeId attachedPinType) {
    if (attachedPinType == GetPinType(inputPinIndex)) {
        return;
    }
    m_pins[inputPinIndex].typeId = attachedPinType;
    if (NeedConvertFunc(inputPinIndex, attachedPinType)) {
        m_pins[inputPinIndex].convertFunc = m_class->GetFuncConvertToDefaultType(inputPinIndex, attachedPinType);
        if (m_pins[inputPinIndex].convertFunc == nullptr) {
            SetConvertError();
        }
    }
}

void Node::DetachFromInputPinCalcType(uint8_t inputPinIndex) {
    m_pins[inputPinIndex].typeId = m_class->GetDefaultPinTypeId(inputPinIndex);
    m_pins[inputPinIndex].convertFunc = nullptr;

    if (ExistsConvertError()) {
        RemoveConvertError();
        for(uint8_t i=InputPinsBeginIndex(); i!=InputPinsEndIndex(); ++i) {
            if ((GetAttachedPinId(i) != 0) && (m_pins[i].convertFunc == nullptr) && NeedConvertFunc(i, m_pins[i].typeId)) {
                SetConvertError();
                break;
            }
        }
    }
}

void Node::DrawGraph(IDraw* drawer) {
    drawer->OnStartDrawNode(static_cast<uintptr_t>(m_id), m_class->GetDisplayName());

    std::vector<IDraw::Pin> pins;
    pins.reserve(InputPinsCount());
    for (uint8_t i=InputPinsBeginIndex(); i!=InputPinsEndIndex(); ++i) {
        pins.emplace_back(IDraw::Pin{ static_cast<uintptr_t>(m_pins[i].id), IsConnectedPin(i), m_class->GetPinDisplayName(i) });
    }
    drawer->OnDrawInputPins(pins);

    if (OutputPinsCount() > 0) {
        auto i = OutputPinsBeginIndex();
        cpgf::GVariant value = m_pins[i].cachedValue;
        if (IsUniversalTypeFromPinId(m_pins[i].id)) {
            value = std::visit([](auto&& v) -> auto {
                return cpgf::copyVariantFromCopyable(v);
            }, cpgf::fromVariant<UniversalType>(value));
        }

        drawer->OnDrawMiniPreview(ToBaseTypeId(GetPinType(i)), value, m_outputValueVersion);
    } else {
        throw EngineError("gs::Node::DrawGraph: not found output pins for draw node (id = {})", m_id);
    }

    pins.clear();
    pins.reserve(OutputPinsCount());
    for (uint8_t i=OutputPinsBeginIndex(); i!=OutputPinsEndIndex(); ++i) {
        pins.emplace_back(IDraw::Pin{ static_cast<uintptr_t>(m_pins[i].id), IsConnectedPin(i), m_class->GetPinDisplayName(i) });
    }
    drawer->OnDrawOutputPins(pins);

    if (m_validFlags == ValidFlags::Valid) {
        drawer->OnFinishDrawNode(true, std::string_view());
    } else if (ExistsResultError()) {
        drawer->OnFinishDrawNode(false, m_lastResultError);
    } else if (ExistsConvertError()) {
        drawer->OnFinishDrawNode(false, "connected pins are of incompatible types");
    } else {
        throw EngineError("gs::Node::DrawGraph: unknown value of m_validFlags");
    }
}

void Node::DrawNodePreview(IDraw* drawer) {
    drawer->OnDrawFullPreview(m_class->GetDisplayName());
}

void Node::DrawNodeProperty(IDraw* drawer) {
    drawer->OnStartDrawNodeProperty(m_class->GetDisplayName());

    for (uint8_t i=EmbeddedPinsBeginIndex(); i!=EmbeddedPinsEndIndex(); ++i) {
        TypeInstanceEdit* typeInstance = m_class->GetTypeInstanceForEmbedded(i);
        if (!typeInstance->IsEnabledUI()) {
            continue;
        }

        typeInstance->Init(m_class->GetValue(i, m_instance));
        IDraw::ButtonsState buttonsState = drawer->OnDrawPinProperty(m_class->GetPinDisplayName(i), typeInstance, false);
        if (typeInstance->IsChanged()) {
            SetEmbeddedValue(i, typeInstance->Result());
        } else if (buttonsState == IDraw::ButtonsState::ResetToDefault) {
            ResetToDefault(i);
        }
    }
    for (uint8_t i=InputPinsBeginIndex(); i!=InputPinsEndIndex(); ++i) {
        const TypeId drawTypeId = ToBaseTypeId(GetPinType(i));
        if (!IsEnableUI(drawTypeId)) {
            continue;
        }

        TypeInstanceEdit* typeInstance = m_class->GetFreeTypeInstance(drawTypeId);
        if (!typeInstance->IsEnabledUI()) {
            continue;
        }

        cpgf::GVariant value = m_class->GetValue(i, m_instance);
        if (IsUniversalTypeFromPinId(m_pins[i].id)) {
            value = std::visit([](auto&& v) -> auto {
                return cpgf::copyVariantFromCopyable(v);
            }, cpgf::fromVariant<UniversalType>(value));
        }

        typeInstance->Init(value);
        IDraw::ButtonsState buttonsState = drawer->OnDrawPinProperty(m_class->GetPinDisplayName(i), typeInstance, IsConnectedPin(i));
        if (typeInstance->IsChanged()) {
            SetInputValue(i, drawTypeId, typeInstance->Result());
        } else if (buttonsState == IDraw::ButtonsState::ResetToDefault) {
            ResetToDefault(i);
        }
    }

    drawer->OnFinishDrawNodeProperty();
}

}
