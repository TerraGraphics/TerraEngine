#pragma once

#include "middleware/generator/shape.h"


class CylinderShape : public Shape {
public:
    CylinderShape() = delete;
    CylinderShape(const UInt2& segments, const Axis& axisUp);
    ~CylinderShape() = default;

protected:
    void FillVertex(VertexBufferRange<VertexPNC>& vb) const override;
    void FillIndex(IndexBufferRange<uint32_t>& ib, uint32_t vertexStartIndex) const override;

private:
    UInt2 m_segments;
    Axis m_axisUp;
};
