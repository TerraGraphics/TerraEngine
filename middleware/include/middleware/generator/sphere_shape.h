#pragma once

#include "core/math/basic.h"
#include "middleware/generator/shape.h"


struct VertexPNC;
template <typename Vertex> class VertexBufferRange;
class SphereShape : public Shape {
public:
    SphereShape() = delete;
    /*!
        Creates a sphere

        The axis of the sphere is directed along axisUp.
        The number of segments along the meridian equals segments.x (segments.x >= 3)
        The number of segments along the axisUp equals segments.y (segments.y >= 3)
        Sphere radius equals 'radius' (radius > 0.f).
        Sphere center is at a point center.
    */
    SphereShape(const math::UInt2& segments, const math::Axis& axisUp, float radius = 0.5f,
        const dg::float3& center = dg::float3(0, 0, 0));

    const UVGridGenerator::Vertexes GetVertexes() const override { return m_generator.GetVertexes(); }
    const UVGridGenerator::Indexes GetIndexes(uint32_t vertexStartIndex = 0) const override { return m_generator.GetIndexes(vertexStartIndex); }

private:
    UVGridGenerator m_generator;
};
