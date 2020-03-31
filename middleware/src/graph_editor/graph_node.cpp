#include "middleware/graph_editor/graph_node.h"

#include <algorithm>


GraphPin::GraphPin(bool isInput, uint32_t pinType, GraphNode* node)
    : isInput(isInput)
    , pinType(pinType)
    , node(node) {

}

GraphNode::GraphNode(dg::IReferenceCounters* refCounters, uint32_t outputPinType, std::initializer_list<uint32_t> inputPinsType)
    : dg::ObjectBase<dg::IObject>(refCounters)
    , m_outputPin(false, outputPinType, this)
    , m_inputPins(inputPinsType.size(), GraphPin(true, 0, this))
    , m_inputs(inputPinsType.size(), Ref()) {

    uint8_t pinNum = 0;
    for (auto pinType: inputPinsType) {
        m_inputPins[pinNum].pinNum = pinNum;
        m_inputPins[pinNum].pinType = pinType;
        ++pinNum;
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

void GraphNode::DetachInput(uint8_t /* number */) {

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
