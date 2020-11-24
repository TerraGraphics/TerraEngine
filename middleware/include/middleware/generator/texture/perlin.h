#pragma once

#include "core/common/ctor.h"
#include "core/math/generator_type_fwd.h"
#include "middleware/generator/texture/noise_types.h"


class Perlin : Fixed {
public:
    Perlin() = default;
    ~Perlin() = default;

    math::Generator3D Result() const;

    double GetFrequency() const { return m_frequency; }
    void SetFrequency(const double v) { m_frequency = v; }

    double GetLacunarity() const { return m_lacunarity; }
    void SetLacunarity(const double v) { m_lacunarity = v; }

    double GetPersistence() const { return m_persistence; }
    void SetPersistence(const double v) { m_persistence = v; }

    int GetOctaveCount() const { return m_octaveCount; }
    void SetOctaveCount(const int v);

    int GetSeed() const { return m_seed; }
    void SetSeed(const int v) { m_seed = v; }

    NoiseQuality GetQuality() const { return m_quality; }
    void SetQuality(const NoiseQuality v) { m_quality = v; }

private:
    double m_frequency = 1.0;
    double m_lacunarity = 2.0;
    double m_persistence = 0.5;
    int m_octaveCount = 6; // 1 - 30
    int m_seed = 0;
    NoiseQuality m_quality = NoiseQuality::Default;
};
