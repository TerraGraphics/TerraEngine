#include "middleware/generator/texture/coherent_noise.h"

Noise3D::Noise3D()
    : GraphNode(0) {

}

CoherentNoise::CoherentNoise() {
    m_generator.SetSeed(1);
}

double CoherentNoise::Get(double x, double y, double z) {
    return m_generator.GetNoise(x, y, z);
}
