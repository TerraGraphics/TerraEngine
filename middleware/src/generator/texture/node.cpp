#include "middleware/generator/texture/node.h"

#include <algorithm>


GraphNode::GraphNode(uint8_t inputPinNumber)
    : m_isFull(inputPinNumber == 0)
    , m_inputs(inputPinNumber, 0) {

}

bool GraphNode::AttachInput(uint8_t number, GraphNode* node) {
    if (number >= m_inputs.size()) {
        return false;
    }

    if (!AttachInputImpl(number, node)) {
        return false;
    }

    m_inputs[number]++;
    m_isFull = (std::count_if(m_inputs.begin(), m_inputs.end(), [](uint16_t v) {return v == 0;}) == 0);

    return true;
}
