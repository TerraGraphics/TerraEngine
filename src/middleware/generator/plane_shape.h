#pragma once

#include "middleware/generator/shape.h"


class PlaneShape : public Shape {
public:
    PlaneShape() = delete;
    PlaneShape(const UInt2& segments, const dg::Vector2<Axis>& axes);
    ~PlaneShape() = default;

protected:
    void FillVertex(VertexBufferRange<VertexPNC>& vb) const override;
    void FillIndex(IndexBufferRange<uint32_t>& ib, uint32_t vertexStartIndex) const override;

private:
    UInt2 m_segments;
    dg::Vector2<Axis> m_axes;
};
