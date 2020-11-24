#pragma once

#include "core/common/ctor.h"
#include "core/math/generator_type_fwd.h"
#include "middleware/generator/texture/noise_types.h"


class Perlin : Fixed {
public:
    Perlin() = default;
    ~Perlin() = default;

    math::Generator3D Result() const;

    NoiseQuality GetQuality() const { return m_quality; }
    void SetQuality(const NoiseQuality v) { m_quality = v; }

private:
    NoiseQuality m_quality = NoiseQuality::Default;
};
