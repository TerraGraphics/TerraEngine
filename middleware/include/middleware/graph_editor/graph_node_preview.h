#pragma once

#include "core/dg/dg.h"
#include "core/math/types.h"
#include "middleware/graph_editor/graph_node.h"


class INodePreview : public GraphNode {
public:
    INodePreview(dg::IReferenceCounters* refCounters, const char* name, uint32_t outputPinType, std::initializer_list<uint32_t> inputPinsType)
        : GraphNode(refCounters, name, outputPinType, inputPinsType) {}

    virtual TexturePtr GetTexture(math::Size size) = 0;
};
