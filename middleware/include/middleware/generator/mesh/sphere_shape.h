#pragma once

#include "core/math/types.h"
#include "middleware/generator/mesh/shape.h"
#include "middleware/generator/mesh/uv_grid_generator.h"
#include "middleware/generator/mesh/vertex_eval_applyer.h"


class SphereShape : public Shape {
public:
    /*!
        Creates a sphere

        Default sphere center at the beginning of coordinates.
        The axis of the sphere is directed along axisUp.
        The number of segments along the meridian equals segments.x (segments.x >= 3)
        The number of segments along the axisUp equals segments.y (segments.y >= 3)
        Sphere radius equals 'radius' (radius > 0.f).
    */
    SphereShape(const dg::uint2 segments, const math::Axis axisUp, float radius = 0.5f);
    SphereShape(SphereShape&& other) noexcept;
    SphereShape& operator=(SphereShape&& other) noexcept;

private:
    VertexEvalApplyer<UVGridGenerator> m_generator;
};
