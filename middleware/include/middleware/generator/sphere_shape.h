#pragma once

#include "core/math/basic.h"
#include "middleware/generator/shape.h"


struct VertexPNC;
template <typename Vertex> class VertexBufferRange;
class SphereShape : public FlatPlaneGenerator {
public:
    SphereShape() = delete;
    /*!
        Creates a sphere with a center at the beginning of coordinates.
        The axis of the sphere is directed along axisUp.
        The number of segments along the meridian equals segments.x (segments.x >= 3)
        The number of segments along the axisUp equals segments.y (segments.y >= 3)
        Sphere radius equals 'radius' (radius > 0.f).
    */
    SphereShape(const UInt2& segments, const math::Axis& axisUp, float radius = 0.5f);
    ~SphereShape() = default;

protected:
    void FillVertex(VertexBufferRange<VertexPNC>& vb) const override;

private:
    math::Axis m_axisUp;
    float m_radius = 0.5f;
};
