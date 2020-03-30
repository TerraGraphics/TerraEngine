#pragma once

#include "middleware/graph_editor/graph_node.h"
#include "middleware/generator/texture/fast_noise.h"


class Noise3D : public GraphNode {
protected:
    Noise3D() = delete;
    Noise3D(dg::IReferenceCounters* refCounters);
    bool AttachInputImpl(uint8_t /* number */, GraphNode* /* node */) override { return false; }

public:
    virtual double Get(double x, double y, double z) = 0;
};

class CoherentNoise : public Noise3D {
public:
    CoherentNoise() = delete;
    CoherentNoise(dg::IReferenceCounters* refCounters);

    double Get(double x, double y, double z) override;

private:
    FastNoise m_generator;
};
