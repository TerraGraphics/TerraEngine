#pragma once

#include "middleware/generator/shape.h"


class ConeShape : public Shape {
public:
    ConeShape() = delete;
    /*!
        Creates a cone with a center at the beginning of coordinates
        The cone is directed along the axisUp axis.
        The number of segments in the cone base is segments.x (segments.x >= 3).
        The number of segments in the cone height is segments.y (segments.y >= 1).
        Cone height equals 'height' (height > 0.f)
        Cone radius equals 'radius' (radius > 0.f)
    */
    ConeShape(const UInt2& segments, const Axis& axisUp = Axis::Y, float height = 1.0f, float radius = 0.5f);
    ~ConeShape() = default;

protected:
    void FillVertex(VertexBufferRange<VertexPNC>& vb) const override;
    void FillIndex(IndexBufferRange<uint32_t>& ib, uint32_t vertexStartIndex) const override;

private:
    UInt2 m_segments;
    Axis m_axisUp;
    float m_height = 0.5f;
    float m_radius = 0.5f;
};
