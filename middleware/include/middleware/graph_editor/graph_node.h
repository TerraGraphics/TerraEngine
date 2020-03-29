#pragma once

#include <vector>
#include <cstdint>

#include "core/dg/dg.h"
#include "core/common/ctor.h"

#include <DiligentCore/Common/interface/ObjectBase.hpp>


class GraphNode : Fixed, public dg::ObjectBase<dg::IObject> {
protected:
    GraphNode() = delete;
    GraphNode(dg::IReferenceCounters* refCounters, uint8_t inputPinNumber);

public:
    bool AttachInput(uint8_t number, GraphNode* node);
    bool IsFull() const noexcept;

    void Draw();

protected:
    virtual bool AttachInputImpl(uint8_t number, GraphNode* node) = 0;

private:
    std::vector<GraphNode*> m_inputs;
};
