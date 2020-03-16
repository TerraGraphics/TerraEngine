#pragma once

#include "core/math/types.h"
#include "middleware/generator/shape.h"
#include "middleware/generator/uv_grid_generator.h"
#include "middleware/generator/vertex_eval_applyer.h"


class PlaneShape : public Shape {
public:
    /*!
        Creates a plane

        Default plane center at the beginning of coordinates.
        The plane is located in the {axes[0], axes[1]} plane, the normals are directed along the "normalDir" axis.
        The number of segments along the axes[N]-axis equals segments[N] (segments[N] >= 1).
        Edge side[N] of the plane is equal to the sizes[N] (sizes[N] > 0).
    */
    PlaneShape(const math::Axis2 axes, math::Direction normalDir,
        const dg::float2 sizes = dg::float2(1.f, 1.f),
        const math::UInt2 segments = {1, 1});

    PlaneShape(PlaneShape&& other) noexcept;
    PlaneShape& operator=(PlaneShape&& other) noexcept;

private:
    VertexEvalApplyer<UVGridGenerator> m_generator;
};
