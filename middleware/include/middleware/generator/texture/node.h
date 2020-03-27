#pragma once

#include <vector>
#include <cstdint>

#include "core/common/ctor.h"


class GraphNode : Fixed {
protected:
    GraphNode() = delete;
    GraphNode(uint8_t inputPinNumber);

public:
    bool AttachInput(uint8_t number, GraphNode* node);
    bool IsFull() const noexcept { return m_isFull; }

protected:
    virtual bool AttachInputImpl(uint8_t number, GraphNode* node) = 0;

private:
    bool m_isFull = false;
    std::vector<uint16_t> m_inputs;
};
