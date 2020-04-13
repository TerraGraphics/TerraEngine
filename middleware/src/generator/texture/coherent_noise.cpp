#include "middleware/generator/texture/coherent_noise.h"


Noise3D::Noise3D(dg::IReferenceCounters* refCounters, const char* name)
    : GraphNode(refCounters, name, Noise3D::OutputTypeID(), {}) {

}

CoherentNoise::CoherentNoise(dg::IReferenceCounters* refCounters)
    : Noise3D(refCounters, GetName()) {

    m_generator.SetNoiseType(FastNoise::PerlinFractal);
}

double CoherentNoise::Get(double x, double y, double z) {
    return m_generator.GetNoise(x, y, z);
}
