#pragma once

#include "middleware/graph_editor/graph_node.h"
#include "middleware/generator/texture/coherent_noise.h"
#include "middleware/generator/texture/texture_consts.h"


class Noise2D : public GraphNode {
protected:
    Noise2D() = delete;
    Noise2D(dg::IReferenceCounters* refCounters, const char* name);
    bool AttachInputImpl(uint8_t number, GraphNode* node) override;
    bool DetachInputImpl(uint8_t number) override;

public:
    static uint32_t OutputTypeID() { return static_cast<uint32_t>(TexturePinType::Noise2D); }

    virtual double Get(double u, double v) = 0;

protected:
    Noise3D* m_noiseNode = nullptr;
};

class PlaneProjection : public Noise2D {
public:
    PlaneProjection() = delete;
    PlaneProjection(dg::IReferenceCounters* refCounters);
    static const char* GetName() { return "Plane projection"; }

    double GetCoordZ() const;
    void SetCoordZ(double value);

    double Get(double u, double v) override;

private:
    double m_coordZ = 0;
};
