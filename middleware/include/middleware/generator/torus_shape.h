#pragma once

#include <cstdint>

#include "core/math/basic.h"
#include "middleware/generator/shape.h"
#include "middleware/generator/uv_grid_generator.h"
#include "middleware/generator/vertex_eval_applyer.h"


class TorusShape : public Shape {
public:
    /*!
        Creates a torus

        Default torus center at the beginning of coordinates.
        The torus is directed along the axisUp axis.
        Torus minor radius equals 'minorRadius' (minorRadius > 0.f).
        Torus major radius equals 'majorRadius' (majorRadius > 0.f).
        The number of segments fo minor radius equals 'minorSegments' (minorSegments >= 3).
        The number of segments fo major radius equals 'majorSegments' (majorSegments >= 3).
    */
    TorusShape(float minorRadius = 0.25f, float majorRadius = 1.0f, uint32_t minorSegments = 10, uint32_t majorSegments = 30,
        const math::Axis axisUp = math::Axis::Z);
    TorusShape(TorusShape&& other) noexcept;
    TorusShape& operator=(TorusShape&& other) noexcept;

private:
    VertexEvalApplyer<UVGridGenerator> m_generator;
};
