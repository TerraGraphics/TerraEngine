#pragma once

#include "middleware/generator/shape.h"


class CylinderShape : public Shape {
public:
    CylinderShape() = delete;
    /*!
        Creates a cylinder with a center at the beginning of coordinates, with a diameter and height equal to 1
        The cylinder is directed along the axisUp axis.
        The number of segments in the cylinder base is segments.x (segments.x >= 3).
        The number of segments in the cylinder height is segments.y (segments.y >= 1).
    */
    CylinderShape(const UInt2& segments, const Axis& axisUp);
    ~CylinderShape() = default;

protected:
    void FillVertex(VertexBufferRange<VertexPNC>& vb) const override;
    void FillIndex(IndexBufferRange<uint32_t>& ib, uint32_t vertexStartIndex) const override;

private:
    UInt2 m_segments;
    Axis m_axisUp;
};
