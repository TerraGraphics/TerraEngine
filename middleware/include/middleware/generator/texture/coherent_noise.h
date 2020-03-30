#pragma once

#include "middleware/graph_editor/graph_node.h"
#include "middleware/generator/texture/fast_noise.h"
#include "middleware/generator/texture/texture_consts.h"


class Noise3D : public GraphNode {
protected:
    Noise3D() = delete;
    Noise3D(dg::IReferenceCounters* refCounters);
    bool AttachInputImpl(uint8_t /* number */, GraphNode* /* node */) override { return false; }

public:
    static uint32_t TypeID() { return static_cast<uint32_t>(TextureGenTypeIDs::Noise3D); }

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
