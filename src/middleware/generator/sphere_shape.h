#pragma once

#include "middleware/generator/shape.h"


class SphereShape : public Shape {
public:
    SphereShape() = delete;
    /*!
        Creates a sphere with a center at the beginning of coordinates and a diameter equal to 1.
        The axis of the sphere is directed along axisUp.
        The number of segments along the meridian equals segments.x (segments.x >= 3)
        The number of segments along the axisUp equals segments.y (segments.y >= 3)
    */
    SphereShape(const UInt2& segments, const Axis& axisUp);
    ~SphereShape() = default;

protected:
    void FillVertex(VertexBufferRange<VertexPNC>& vb) const override;
    void FillIndex(IndexBufferRange<uint32_t>& ib, uint32_t vertexStartIndex) const override;

private:
    UInt2 m_segments;
    Axis m_axisUp;
};
