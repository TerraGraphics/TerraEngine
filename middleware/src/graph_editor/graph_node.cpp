#include "middleware/graph_editor/graph_node.h"

#include <algorithm>


GraphNode::GraphNode(dg::IReferenceCounters* refCounters, uint8_t inputPinNumber)
    : dg::ObjectBase<dg::IObject>(refCounters)
    , m_inputs(inputPinNumber, nullptr) {

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
    for (auto* pin : m_inputs) {
        if ((!pin) || (!pin->IsFull())) {
            return false;
        }
    }

    return true;
}

void GraphNode::Draw() {

}