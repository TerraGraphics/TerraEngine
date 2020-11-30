#include "middleware/generator/texture/coherent_noise.h"

#include <algorithm>

#include "noise/fast_noise.h"
#include "core/math/generator_type.h"


CoherentNoise::CoherentNoise()
    : m_noise(new FastNoiseLite()) {

    SetSeed(m_seed);
    SetFrequency(m_frequency);
    SetNoiseType(m_noiseType);
    // SetRotationType3D
    SetFractalType(FractalType::FBm);
    SetOctaveCount(m_octaveCount);
    SetLacunarity(m_lacunarity);
    SetGain(m_gain);
    // SetFractalWeightedStrength
    // SetFractalPingPongStrength
    // SetCellularDistanceFunction
    // SetCellularReturnType
    // SetCellularJitter
    // SetDomainWarpType
    // SetDomainWarpAmp
}

math::Generator3D CoherentNoise::Result() const {
    return math::Generator3D([noise = m_noise](double x, double y, double z) -> double {
        return noise->GetNoise<double>(x, y, z);
    });
}

void CoherentNoise::SetNoiseType(const NoiseType v) {
    m_noiseType = v;
    switch (v) {
    case NoiseType::OpenSimplex2:
        m_noise->SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
        break;
    case NoiseType::OpenSimplex2S:
        m_noise->SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2S);
        break;
    case NoiseType::Cellular:
        m_noise->SetNoiseType(FastNoiseLite::NoiseType_Cellular);
        break;
    case NoiseType::Perlin:
        m_noise->SetNoiseType(FastNoiseLite::NoiseType_Perlin);
        break;
    case NoiseType::ValueCubic:
        m_noise->SetNoiseType(FastNoiseLite::NoiseType_ValueCubic);
        break;
    case NoiseType::Value:
        m_noise->SetNoiseType(FastNoiseLite::NoiseType_Value);
        break;
    }
}

void CoherentNoise::SetFractalType(const FractalType v) {
    m_fractalType = v;
    switch (v) {
    case FractalType::None:
        m_noise->SetFractalType(FastNoiseLite::FractalType_None);
        break;
    case FractalType::FBm:
        m_noise->SetFractalType(FastNoiseLite::FractalType_FBm);
        break;
    case FractalType::Ridged:
        m_noise->SetFractalType(FastNoiseLite::FractalType_Ridged);
        break;
    case FractalType::PingPong:
        m_noise->SetFractalType(FastNoiseLite::FractalType_PingPong);
        break;
    }
}

void CoherentNoise::SetOctaveCount(const uint8_t v) {
    m_octaveCount = std::clamp(v, uint8_t(1), uint8_t(30));
    m_noise->SetFractalOctaves(static_cast<int>(m_octaveCount));
}

void CoherentNoise::SetSeed(const int v) {
    m_seed = v;
    m_noise->SetSeed(v);
}

void CoherentNoise::SetFrequency(const float v) {
    m_frequency = v;
    m_noise->SetFrequency(v);
}

void CoherentNoise::SetLacunarity(const float v) {
    m_lacunarity = v;
    m_noise->SetFractalLacunarity(v);
}

void CoherentNoise::SetGain(const float v) {
    m_gain = v;
    m_noise->SetFractalGain(v);
}
