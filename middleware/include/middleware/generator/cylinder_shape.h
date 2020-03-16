#pragma once

#include "core/math/types.h"
#include "middleware/generator/shape.h"
#include "middleware/generator/uv_grid_generator.h"
#include "middleware/generator/vertex_eval_applyer.h"


class CylinderShape : public Shape {
public:
    /*!
        Creates a cylinder

        Default cylinder center at the beginning of coordinates.
        The cylinder is directed along the axisUp axis.
        The number of segments in the cylinder base is segments.x (segments.x >= 3).
        The number of segments in the cylinder height is segments.y (segments.y >= 1).
        Cylinder radius equals 'radius' (radius > 0.f).
        Cylinder height equals 'height' (height > 0.f).
    */
    CylinderShape(const math::UInt2 segments, const math::Axis axisUp, float radius = 0.5f, float height = 1.0f);
    CylinderShape(CylinderShape&& other) noexcept;
    CylinderShape& operator=(CylinderShape&& other) noexcept;

private:
    VertexEvalApplyer<UVGridGenerator> m_generator;
};
