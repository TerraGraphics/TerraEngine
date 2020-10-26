#pragma once

#include <cstdint>

#include "middleware/graph_editor/graph_node.h"


namespace Diligent {
    class IReferenceCounters;
}
class Noise3D;
class Noise2D : public GraphNode {
protected:
    Noise2D() = delete;
    Noise2D(dg::IReferenceCounters* refCounters, const char* name);
    bool AttachInputImpl(uint8_t number, GraphNode* node) override;
    bool DetachInputImpl(uint8_t number) override;

public:
    static uint32_t OutputTypeID();

    virtual double Get(double u, double v) = 0;

protected:
    Noise3D* m_noiseNode = nullptr;
};
