#pragma once

#include "core/math/basic.h"
#include "middleware/generator/shape.h"


struct VertexPNC;
template <typename Vertex> class VertexBufferRange;
class CylinderShape : public Shape {
public:
    CylinderShape() = delete;
    /*!
        Creates a cylinder

        The cylinder is directed along the axisUp axis.
        The number of segments in the cylinder base is segments.x (segments.x >= 3).
        The number of segments in the cylinder height is segments.y (segments.y >= 1).
        Cylinder radius equals 'radius' (radius > 0.f).
        Cylinder height equals 'height' (height > 0.f).
        Cylinder center is at a point center.
    */
    CylinderShape(const math::UInt2& segments, const math::Axis& axisUp, float radius = 0.5f, float height = 1.0f,
        const dg::float3& center = dg::float3(0, 0, 0));

    const UVGridGenerator::Vertexes GetVertexes() const override { return m_generator.GetVertexes(); }
    const UVGridGenerator::Indexes GetIndexes(uint32_t vertexStartIndex = 0) const override { return m_generator.GetIndexes(vertexStartIndex); }

private:
    UVGridGenerator m_generator;
};
