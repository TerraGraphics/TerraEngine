#pragma once

#include "core/common/ctor.h"
#include "core/math/generator_type_fwd.h"


class Spheres : Fixed {
public:
    Spheres() = default;
    ~Spheres() = default;

    math::Generator3D Result() const;

    double GetFrequency() const { return m_frequency; }
    void SetFrequency(const double v) { m_frequency = v; }

private:
    double m_frequency = 1.;
};
