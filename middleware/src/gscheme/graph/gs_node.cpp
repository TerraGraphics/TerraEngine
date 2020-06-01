#include "middleware/gscheme/graph/gs_node.h"

#include "core/common/exception.h"
#include "middleware/gscheme/graph/gs_id.h"
#include "middleware/gscheme/graph/gs_limits.h"
#include "middleware/gscheme/graph/gs_type_class.h"


namespace gs {

static_assert(sizeof(Pin) == 16, "sizeof(Pin) == 16 bytes");
static_assert(sizeof(Node) == 32, "sizeof(Node) == 32 bytes");

void Node::Init(uint16_t id) noexcept {
    m_id = id;
    // next free node index
    m_nextIndex = id;
}

void Node::Create(TypeClass* typeClass) {
    m_typeClass = typeClass;

    m_countEmbeddedPins = m_typeClass->EmbeddedPinsCount();
    m_countInputPins = m_typeClass->InputPinsCount();
    m_countOutputPins = m_typeClass->OutputPinsCount();
    m_pins = new Pin[m_countEmbeddedPins + m_countInputPins + m_countOutputPins];
    uint32_t baseID = static_cast<uint32_t>(m_id) << uint32_t(16);

    uint32_t typePin = 3; // embeded
    for(uint8_t i=EmbededPinsBeginIndex(); i!=EmbededPinsEndIndex(); ++i) {
        m_pins[i].id = baseID | (static_cast<uint32_t>(i) << uint32_t(8)) | typePin;
    }

    typePin = 1; // input
    for(uint8_t i=InputPinsBeginIndex(); i!=InputPinsEndIndex(); ++i) {
        m_pins[i].id = baseID | (static_cast<uint32_t>(i) << uint32_t(8)) | typePin;
    }

    typePin = 0; // output
    for(uint8_t i=OutputPinsBeginIndex(); i!=OutputPinsEndIndex(); ++i) {
        m_pins[i].id = baseID | (static_cast<uint32_t>(i) << uint32_t(8)) | typePin;
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

void Node::ResetOrder() noexcept {
    m_order = 0;
    m_nextIndex = INVALID_NODE_INDEX;
}

bool Node::IsExistsConnectedOutputPins() const noexcept {
    for(uint8_t i=OutputPinsBeginIndex(); i!=OutputPinsEndIndex(); ++i) {
        if (m_pins[i].linksCount != 0) {
            return true;
        }
    }

    return false;
}

void Node::CheckIsValidEmbededPinIndex(uint8_t pinIndex) const {
    if (m_countEmbeddedPins == 0) {
        throw EngineError("no embedded pins");
    }

    if (EmbededPinsBeginIndex() > pinIndex) {
        throw EngineError("min value = {}", EmbededPinsBeginIndex());
    }

    if (EmbededPinsEndIndex() <= pinIndex) {
        throw EngineError("max value = {}", EmbededPinsEndIndex() - 1);
    }
}

void Node::CheckIsValidInputPinIndex(uint8_t pinIndex) const {
    if (m_countInputPins == 0) {
        throw EngineError("no input pins");
    }

    if (InputPinsBeginIndex() > pinIndex) {
        throw EngineError("min value = {}", InputPinsBeginIndex());
    }

    if (InputPinsEndIndex() <= pinIndex) {
        throw EngineError("max value = {}", InputPinsEndIndex() - 1);
    }
}

void Node::CheckIsValidOutputPinIndex(uint8_t pinIndex) const {
    if (m_countOutputPins == 0) {
        throw EngineError("no output pins");
    }

    if (OutputPinsBeginIndex() > pinIndex) {
        throw EngineError("min value = {}", OutputPinsBeginIndex());
    }

    if (OutputPinsEndIndex() <= pinIndex) {
        throw EngineError("max value = {}", OutputPinsEndIndex() - 1);
    }
}

uint16_t Node::GetOrderNumber(Node* nodes) noexcept {
    if (m_order == 0) {
        m_order = 1;
        uint16_t maxAttachedNodeIndex = INVALID_NODE_INDEX;
        for(uint8_t i=InputPinsBeginIndex(); i!=InputPinsEndIndex(); ++i) {
            if (m_pins[i].attachedPinID != 0) {
                uint16_t nodeIndex = NodeIndexFromPinId(m_pins[i].attachedPinID);
                uint16_t order = nodes[nodeIndex].GetOrderNumber(nodes) + 1;
                if (order > m_order) {
                    m_order = order;
                    maxAttachedNodeIndex = nodeIndex;
                }
            }
        }
        if (m_order > 1) {
            m_nextIndex = nodes[maxAttachedNodeIndex].SetNextCalcIndex(m_id - 1);
        }
    }

    return m_order;
}

uint16_t Node::SetNextCalcIndex(uint16_t nodeIndex) noexcept {
    uint16_t result = m_nextIndex;
    m_nextIndex = nodeIndex;

    return result;
}

void Node::ResetAcyclicityChecked() noexcept {
    m_isAcyclicityChecked = false;
}

bool Node::CheckAcyclicity(Node* nodes, uint16_t startNodeId) noexcept {
    if (m_isAcyclicityChecked) {
        return true;
    }
    m_isAcyclicityChecked = true;
    if (m_id == startNodeId) {
        return false;
    }

    for(uint8_t i=InputPinsBeginIndex(); i!=InputPinsEndIndex(); ++i) {
        if (m_pins[i].attachedPinID != 0) {
            if (!nodes[NodeIndexFromPinId(m_pins[i].attachedPinID)].CheckAcyclicity(nodes, startNodeId)) {
                return false;
            }
        }
    }

    return true;
}

void Node::AttachToInputPin(uint8_t inputPinIndex, uint32_t attachedPinID) noexcept {
    m_pins[inputPinIndex].attachedPinID = attachedPinID;
}

void Node::DetachFromInputPin(uint8_t inputPinIndex) noexcept {
    m_pins[inputPinIndex].attachedPinID = 0;
}

void Node::DetachFromInputPinIfExists(uint16_t attachedNodeID) noexcept {
    for(uint8_t i=InputPinsBeginIndex(); i!=InputPinsEndIndex(); ++i) {
        if (NodeIdFromPinId(m_pins[i].attachedPinID) == attachedNodeID) {
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
