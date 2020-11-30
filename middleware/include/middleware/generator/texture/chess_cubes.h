#pragma once

#include "core/common/ctor.h"
#include "core/math/generator_type_fwd.h"


class ChessCubes : Fixed {
public:
    ChessCubes() = default;
    ~ChessCubes() = default;

    math::Generator3D Result() const;

    float GetFrequency() const { return m_frequency; }
    void SetFrequency(const float v) { m_frequency = v; }

private:
    float m_frequency = 1.f;
};
