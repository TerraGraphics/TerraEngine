#pragma once

#include "core/math/basic.h"
#include "middleware/generator/shape.h"
#include "middleware/generator/merge_shape.h"
#include "middleware/generator/plane_shape.h"


class CubeShape : public Shape {
public:
    /*!
        Creates a cube

        Default cube center at the beginning of coordinates.
        Edge sides of the cube is equal to "sizes" (sizes[N] > 0).
        The number of segments by axis is equal to "segments" (segments.N >= 3).
    */
    CubeShape(const dg::float3 sizes = dg::float3(1.f, 1.f, 1.f), const math::UInt3 segments = {1, 1, 1});
    CubeShape(CubeShape&& other) noexcept;
    CubeShape& operator=(CubeShape&& other) noexcept;

private:
    MergeShape<PlaneShape, PlaneShape, PlaneShape, PlaneShape, PlaneShape, PlaneShape> m_generator;
};
