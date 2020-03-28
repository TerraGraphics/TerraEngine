#pragma once

#include "core/common/ctor.h"
#include "middleware/generator/texture/fast_noise.h"


class CoherentNoise : Fixed {
public:
    CoherentNoise();

    double Get(double x, double y, double z);

private:
    FastNoise m_generator;
};