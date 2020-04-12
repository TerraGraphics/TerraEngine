#pragma once

#define FN_USE_DOUBLES
#include <FastNoise.h>

#include "middleware/graph_editor/graph_node.h"
#include "middleware/generator/texture/texture_consts.h"


class Noise3D : public GraphNode {
protected:
    Noise3D() = delete;
    Noise3D(dg::IReferenceCounters* refCounters);
    bool AttachInputImpl(uint8_t /* number */, GraphNode* /* node */) override { return false; }
    bool DetachInputImpl(uint8_t /* number */) override { return false; }

public:
    static uint32_t OutputTypeID() { return static_cast<uint32_t>(TexturePinType::Noise3D); }

    virtual double Get(double x, double y, double z) = 0;
};

class CoherentNoise : public Noise3D {
public:
    CoherentNoise() = delete;
    CoherentNoise(dg::IReferenceCounters* refCounters);
    static const char* GetName() { return "Coherent noise"; }

    double Get(double x, double y, double z) override;

private:
    FastNoise m_generator;
};
