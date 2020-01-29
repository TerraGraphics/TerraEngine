#pragma once

#include "middleware/generator/shape.h"


class SphereShape : public Shape {
public:
    SphereShape() = delete;
    SphereShape(const UInt2& segments, const Axis& axisUp);
    ~SphereShape() = default;

protected:
    void FillVertex(VertexBufferRange<VertexPNC>& vb) const override;
    void FillIndex(IndexBufferRange<uint32_t>& ib, uint32_t vertexStartIndex) const override;

private:
    UInt2 m_segments;
    Axis m_axisUp;
};
