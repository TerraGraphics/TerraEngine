#include "middleware/generator/texture/coherent_noise.h"

#include <algorithm>

#include "noise/fast_noise.h"
#include "core/math/generator_type.h"


math::Generator3D CoherentNoise::Result() const {
    FastNoiseLite noise;
    noise.SetSeed(m_seed);
    noise.SetFrequency(m_frequency);
    switch (m_noiseType) {
    case NoiseType::OpenSimplex2:
        noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
        break;
    case NoiseType::OpenSimplex2S:
        noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2S);
        break;
    case NoiseType::Cellular:
        noise.SetNoiseType(FastNoiseLite::NoiseType_Cellular);
        break;
    case NoiseType::Perlin:
        noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
        break;
    case NoiseType::ValueCubic:
        noise.SetNoiseType(FastNoiseLite::NoiseType_ValueCubic);
        break;
    case NoiseType::Value:
        noise.SetNoiseType(FastNoiseLite::NoiseType_Value);
        break;
    }
    // noise.SetRotationType3D
    switch (m_fractalType) {
    case FractalType::None:
        noise.SetFractalType(FastNoiseLite::FractalType_None);
        break;
    case FractalType::FBm:
        noise.SetFractalType(FastNoiseLite::FractalType_FBm);
        break;
    case FractalType::Ridged:
        noise.SetFractalType(FastNoiseLite::FractalType_Ridged);
        break;
    case FractalType::PingPong:
        noise.SetFractalType(FastNoiseLite::FractalType_PingPong);
        break;
    }
    noise.SetFractalOctaves(static_cast<int>(m_octaveCount));
    noise.SetFractalLacunarity(m_lacunarity);
    noise.SetFractalGain(m_gain);
    // SetFractalWeightedStrength
    noise.SetFractalPingPongStrength(m_pingPongStrength);
    switch (m_cellularDistanceFunction) {
    case CellularDistanceFunction::Euclidean:
        noise.SetCellularDistanceFunction(FastNoiseLite::CellularDistanceFunction_Euclidean);
        break;
    case CellularDistanceFunction::EuclideanSq:
        noise.SetCellularDistanceFunction(FastNoiseLite::CellularDistanceFunction_EuclideanSq);
        break;
    case CellularDistanceFunction::Manhattan:
        noise.SetCellularDistanceFunction(FastNoiseLite::CellularDistanceFunction_Manhattan);
        break;
    case CellularDistanceFunction::Hybrid:
        noise.SetCellularDistanceFunction(FastNoiseLite::CellularDistanceFunction_Hybrid);
        break;
    }
    switch (m_cellularReturnType) {
    case CellularReturnType::CellValue:
        noise.SetCellularReturnType(FastNoiseLite::CellularReturnType_CellValue);
        break;
    case CellularReturnType::Distance:
        noise.SetCellularReturnType(FastNoiseLite::CellularReturnType_Distance);
        break;
    case CellularReturnType::Distance2:
        noise.SetCellularReturnType(FastNoiseLite::CellularReturnType_Distance2);
        break;
    case CellularReturnType::Distance2Add:
        noise.SetCellularReturnType(FastNoiseLite::CellularReturnType_Distance2Add);
        break;
    case CellularReturnType::Distance2Sub:
        noise.SetCellularReturnType(FastNoiseLite::CellularReturnType_Distance2Sub);
        break;
    case CellularReturnType::Distance2Mul:
        noise.SetCellularReturnType(FastNoiseLite::CellularReturnType_Distance2Mul);
        break;
    case CellularReturnType::Distance2Div:
        noise.SetCellularReturnType(FastNoiseLite::CellularReturnType_Distance2Div);
        break;
    }
    noise.SetCellularJitter(m_cellularJitter);
    // SetDomainWarpType
    // SetDomainWarpAmp
    return math::Generator3D([noise](double x, double y, double z) mutable -> double {
        return noise.GetNoise<double>(x, y, z);
    });
}

void CoherentNoise::SetOctaveCount(const uint8_t v) {
    m_octaveCount = std::clamp(v, uint8_t(1), uint8_t(30));
}

void CoherentNoise::SetCellularJitter(const float v) {
    m_cellularJitter = std::clamp(v, 0.f, 1.f);
}
