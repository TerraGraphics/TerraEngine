#pragma once

#include "core/common/ctor.h"
#include "core/math/generator_type_fwd.h"
#include "middleware/generator/texture/noise_types.h"


enum class NoiseType : uint8_t {
    OpenSimplex2 = 0,
    OpenSimplex2S = 1,
    Cellular = 2,
    Perlin = 3,
    ValueCubic = 4,
    Value = 5
};

enum class FractalType : uint8_t {
    None = 0,
    FBm = 1,
    Ridged = 2,
    PingPong = 3,
    // FractalType_DomainWarpProgressive = 4,
    // FractalType_DomainWarpIndependent = 5
};

class CoherentNoise : Fixed {
public:
    CoherentNoise() = default;
    ~CoherentNoise() = default;

    math::Generator3D Result() const;

    NoiseType GetNoiseType() const { return m_noiseType; }
    void SetNoiseType(const NoiseType v) { m_noiseType = v; }

    FractalType GetFractalType() const { return m_fractalType; }
    void SetFractalType(const FractalType v) { m_fractalType = v; }

    uint8_t GetOctaveCount() const { return m_octaveCount; }
    // range: [1 - 30]
    void SetOctaveCount(const uint8_t v);

    int GetSeed() const { return m_seed; }
    void SetSeed(const int v) { m_seed = v; }

    float GetFrequency() const { return m_frequency; }
    void SetFrequency(const float v) { m_frequency = v; }

    float GetLacunarity() const { return m_lacunarity; }
    void SetLacunarity(const float v) { m_lacunarity = v; }

    float GetGain() const { return m_gain; }
    void SetGain(const float v) { m_gain = v; }

private:
    NoiseType m_noiseType = NoiseType::OpenSimplex2;
    FractalType m_fractalType = FractalType::FBm;
    uint8_t m_octaveCount = 3;
    int m_seed = 1337;
    float m_frequency = 1.f;
    float m_lacunarity = 2.f;
    float m_gain = 0.5f;
};
