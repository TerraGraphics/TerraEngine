#include "middleware/generator/texture/coherent_noise.h"


Noise3D::Noise3D(dg::IReferenceCounters* refCounters)
    : GraphNode(refCounters, 0) {

}

CoherentNoise::CoherentNoise(dg::IReferenceCounters* refCounters)
    : Noise3D(refCounters) {

    m_generator.SetNoiseType(FastNoise::PerlinFractal);
}

double CoherentNoise::Get(double x, double y, double z) {
    return m_generator.GetNoise(x, y, z);
}
