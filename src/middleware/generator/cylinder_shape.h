#pragma once

#include "middleware/generator/shape.h"


class CylinderShape : public Shape {
public:
    CylinderShape() = delete;
    /*!
        Creates a cylinder with a center at the beginning of coordinates.
        The cylinder is directed along the axisUp axis.
        The number of segments in the cylinder base is segments.x (segments.x >= 3).
        The number of segments in the cylinder height is segments.y (segments.y >= 1).
        Cylinder radius equals 'radius' (radius > 0.f).
        Cylinder height equals 'height' (height > 0.f).
    */
    CylinderShape(const UInt2& segments, const Axis& axisUp, float radius = 0.5f, float height = 1.0f);
    ~CylinderShape() = default;

protected:
    void FillVertex(VertexBufferRange<VertexPNC>& vb) const override;
    void FillIndex(IndexBufferRange<uint32_t>& ib, uint32_t vertexStartIndex) const override;

private:
    UInt2 m_segments;
    Axis m_axisUp;
    float m_radius = 0.5f;
    float m_height = 1.0f;
};
