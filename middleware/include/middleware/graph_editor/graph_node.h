#pragma once

#include <map>
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
    GraphNode(dg::IReferenceCounters* refCounters, const char* name, uint32_t outputPinType, std::initializer_list<uint32_t> inputPinsType);
    virtual ~GraphNode();

public:
    using Ref = dg::RefCntAutoPtr<GraphNode>;
    using Weak = dg::RefCntWeakPtr<GraphNode>;

    bool IsFull() const noexcept;

    bool AttachInput(uint8_t number, GraphNode* node);
    bool DetachInput(uint8_t number);

    void Draw(uint8_t alpha, TextureViewRaw texBackground, float texWidth, float texHeight);

protected:
    virtual void StateChanged();
    bool GetIsDirty() { return m_isDirty; }
    void ResetIsDirty() { m_isDirty = false; }

    virtual bool AttachInputImpl(uint8_t number, GraphNode* node) = 0;
    virtual bool DetachInputImpl(uint8_t number) = 0;

private:
    void AttachOutput(GraphNode* node);
    void DetachOutput(GraphNode* node);

private:
    const char* m_name;
    bool m_isDirty = true;
    GraphPin m_outputPin;
    std::vector<GraphPin> m_inputPins;
    std::vector<Ref> m_inputs;
    std::map<GraphNode*, Weak> m_outputs;
};
