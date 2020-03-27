#pragma once

#include "middleware/generator/texture/node.h"
#include "middleware/generator/texture/coherent_noise.h"


class Noise2D : public GraphNode {
protected:
    Noise2D();
    bool AttachInputImpl(uint8_t number, GraphNode* node) override;

public:
    virtual double Get(double u, double v) = 0;

protected:
    Noise3D* m_noiseNode = nullptr;
};

class PlaneProjection : public Noise2D {
public:
    double Get(double u, double v) override;

private:
    double m_coordZ = 0;
};
