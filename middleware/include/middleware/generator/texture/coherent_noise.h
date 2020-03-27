#pragma once

#include "middleware/generator/texture/node.h"
#include "middleware/generator/texture/fast_noise.h"


class Noise3D : GraphNode {
protected:
    Noise3D();
    bool AttachInputImpl(uint8_t /* number */, GraphNode* /* node */) override { return false; }

public:
    virtual double Get(double x, double y, double z) = 0;
};

class CoherentNoise : public Noise3D {
public:
    CoherentNoise();

    double Get(double x, double y, double z) override;

private:
    FastNoise m_generator;
};
