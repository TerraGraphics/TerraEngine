#include "middleware/generator/texture/node.h"

#include <algorithm>


GraphNode::GraphNode(uint8_t inputPinNumber)
    : m_inputs(inputPinNumber, nullptr) {

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
