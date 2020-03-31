#include "middleware/graph_editor/graph_node.h"

#include <algorithm>


GraphPin::GraphPin(bool isInput, uint32_t pinType, GraphNode* parent)
    : isInput(isInput)
    , pinType(pinType)
    , parent(parent) {

}

GraphNode::GraphNode(dg::IReferenceCounters* refCounters, uint32_t outputPinType, std::initializer_list<uint32_t> inputPinsType)
    : dg::ObjectBase<dg::IObject>(refCounters)
    , m_outputPin(false, outputPinType, this)
    , m_inputPins(inputPinsType.size(), GraphPin(true, 0, this))
    , m_inputs(inputPinsType.size(), Ref()) {

    uint i = 0;
    for (auto pinType: inputPinsType) {
        m_inputPins[i++].pinType = pinType;
    }
}

bool GraphNode::AttachInput(uint8_t number, GraphNode* node) {
    if (number >= m_inputs.size()) {
        return false;
    }

    if (!AttachInputImpl(number, node)) {
        return false;
    }

    m_inputs[number] = node;

    return true;
}

bool GraphNode::IsFull() const noexcept {
    for (const auto& pin : m_inputs) {
        if ((!pin) || (!pin->IsFull())) {
            return false;
        }
    }

    return true;
}

void GraphNode::Draw() {

}
