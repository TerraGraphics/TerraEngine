#pragma once

#include "middleware/graph_editor/graph_node.h"
#include "middleware/generator/texture/coherent_noise.h"
#include "middleware/generator/texture/texture_consts.h"


class Noise2D : public GraphNode {
protected:
    Noise2D() = delete;
    Noise2D(dg::IReferenceCounters* refCounters);
    bool AttachInputImpl(uint8_t number, GraphNode* node) override;

public:
    static uint32_t TypeID() { return static_cast<uint32_t>(TextureGenTypeIDs::Noise2D); }

    virtual double Get(double u, double v) = 0;

protected:
    Noise3D* m_noiseNode = nullptr;
};

class PlaneProjection : public Noise2D {
public:
    PlaneProjection() = delete;
    PlaneProjection(dg::IReferenceCounters* refCounters);

    double Get(double u, double v) override;

private:
    double m_coordZ = 0;
};
