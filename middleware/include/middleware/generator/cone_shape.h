#pragma once

#include "core/math/basic.h"
#include "middleware/generator/shape.h"
#include "middleware/generator/vertex_eval_applyer.h"


class ConeShape : public Shape {
public:
    ConeShape() = delete;
    /*!
        Creates a cone

        Default cone center at the beginning of coordinates.
        The cone is directed along the axisUp axis.
        The number of segments in the cone base is segments.x (segments.x >= 3).
        The number of segments in the cone height is segments.y (segments.y >= 1).
        Cone radius equals 'radius' (radius > 0.f).
        Cone height equals 'height' (height > 0.f).
    */
    ConeShape(const math::UInt2 segments, const math::Axis axisUp = math::Axis::Y, float radius = 0.5f, float height = 1.0f);

private:
    VertexEvalApplyer<UVGridGenerator> m_generator;
};
