#pragma once

#include <vector>
#include <cstdint>

#include "core/dg/dg.h"
#include "core/common/ctor.h"

#include <DiligentCore/Common/interface/ObjectBase.hpp>


class GraphNode;
struct GraphPin {
    GraphPin(bool isInput, uint32_t pinType, GraphNode* node);

    bool isInput = true;
    bool isConnected = false;
    uint8_t pinNum = 0;
    uint32_t pinType = 0;
    GraphNode* node = nullptr;
};

class GraphNode : Fixed, public dg::ObjectBase<dg::IObject> {
protected:
    GraphNode() = delete;
    GraphNode(dg::IReferenceCounters* refCounters, uint32_t outputPinType, std::initializer_list<uint32_t> inputPinsType);

public:
    using Ref = dg::RefCntAutoPtr<GraphNode>;

    bool AttachInput(uint8_t number, GraphNode* node);
    void DetachInput(uint8_t number);
    bool IsFull() const noexcept;

    void Draw();

protected:
    virtual bool AttachInputImpl(uint8_t number, GraphNode* node) = 0;

private:
    GraphPin m_outputPin;
    std::vector<GraphPin> m_inputPins;
    std::vector<Ref> m_inputs;
};
