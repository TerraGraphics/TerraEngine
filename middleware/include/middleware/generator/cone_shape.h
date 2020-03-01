#pragma once

#include "core/math/basic.h"
#include "middleware/generator/shape.h"


struct VertexPNC;
template <typename Vertex> class VertexBufferRange;
class ConeShape : public FlatPlaneGenerator {
public:
    ConeShape() = delete;
    /*!
        Creates a cone with a center at the beginning of coordinates.
        The cone is directed along the axisUp axis.
        The number of segments in the cone base is segments.x (segments.x >= 3).
        The number of segments in the cone height is segments.y (segments.y >= 1).
        Cone radius equals 'radius' (radius > 0.f).
        Cone height equals 'height' (height > 0.f).
    */
    ConeShape(const math::UInt2& segments, const math::Axis& axisUp = math::Axis::Y, float radius = 0.5f, float height = 1.0f);
    ~ConeShape() = default;

protected:
    void FillVertex(VertexBufferRange<VertexPNC>& vb) const override;

private:
    math::Axis m_axisUp;
    float m_radius = 0.5f;
    float m_height = 1.0f;
};
