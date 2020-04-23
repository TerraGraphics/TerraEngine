#include "middleware/generator/texture/coherent_noise.h"

#include "core/common/exception.h"


namespace {

static FastNoise::NoiseType ToFastNoise(CoherentNoise::NoiseType value) {
    switch (value) {
    case CoherentNoise::NoiseType::Value:
        return FastNoise::NoiseType::Value;
    case CoherentNoise::NoiseType::ValueFractal:
        return FastNoise::NoiseType::ValueFractal;
    case CoherentNoise::NoiseType::Perlin:
        return FastNoise::NoiseType::Perlin;
    case CoherentNoise::NoiseType::PerlinFractal:
        return FastNoise::NoiseType::PerlinFractal;
    case CoherentNoise::NoiseType::Simplex:
        return FastNoise::NoiseType::Simplex;
    case CoherentNoise::NoiseType::SimplexFractal:
        return FastNoise::NoiseType::SimplexFractal;
    case CoherentNoise::NoiseType::Cellular:
        return FastNoise::NoiseType::Cellular;
    case CoherentNoise::NoiseType::WhiteNoise:
        return FastNoise::NoiseType::WhiteNoise;
    case CoherentNoise::NoiseType::Cubic:
        return FastNoise::NoiseType::Cubic;
    case CoherentNoise::NoiseType::CubicFractal:
        return FastNoise::NoiseType::CubicFractal;
    default:
        throw EngineError("Unknown CoherentNoise::NoiseType value = {}", value);
    }
}

static CoherentNoise::NoiseType FromFastNoise(FastNoise::NoiseType value) {
    switch (value) {
    case FastNoise::NoiseType::Value:
        return CoherentNoise::NoiseType::Value;
    case FastNoise::NoiseType::ValueFractal:
        return CoherentNoise::NoiseType::ValueFractal;
    case FastNoise::NoiseType::Perlin:
        return CoherentNoise::NoiseType::Perlin;
    case FastNoise::NoiseType::PerlinFractal:
        return CoherentNoise::NoiseType::PerlinFractal;
    case FastNoise::NoiseType::Simplex:
        return CoherentNoise::NoiseType::Simplex;
    case FastNoise::NoiseType::SimplexFractal:
        return CoherentNoise::NoiseType::SimplexFractal;
    case FastNoise::NoiseType::Cellular:
        return CoherentNoise::NoiseType::Cellular;
    case FastNoise::NoiseType::WhiteNoise:
        return CoherentNoise::NoiseType::WhiteNoise;
    case FastNoise::NoiseType::Cubic:
        return CoherentNoise::NoiseType::Cubic;
    case FastNoise::NoiseType::CubicFractal:
        return CoherentNoise::NoiseType::CubicFractal;
    default:
        throw EngineError("Unknown FastNoise::NoiseType value = {}", value);
    }
}

}

Noise3D::Noise3D(dg::IReferenceCounters* refCounters, const char* name)
    : GraphNode(refCounters, name, Noise3D::OutputTypeID(), {}) {

}

CoherentNoise::CoherentNoise(dg::IReferenceCounters* refCounters)
    : Noise3D(refCounters, GetName()) {

    m_generator.SetNoiseType(FastNoise::PerlinFractal);
}

CoherentNoise::NoiseType CoherentNoise::GetNoiseType() const {
    return FromFastNoise(m_generator.GetNoiseType());
}

void CoherentNoise::SetNoiseType(NoiseType value) {
    m_generator.SetNoiseType(ToFastNoise(value));
    StateChanged();
}

int CoherentNoise::GetSeed() const {
    return m_generator.GetSeed();
}

void CoherentNoise::SetSeed(int value) {
    m_generator.SetSeed(value);
    StateChanged();
}

double CoherentNoise::Get(double x, double y, double z) {
    return m_generator.GetNoise(x, y, z);
}
