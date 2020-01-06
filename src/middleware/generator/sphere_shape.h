#pragma once

#include "middleware/generator/shape.h"


class SphereShape : public Shape {
public:
    SphereShape() = delete;
    SphereShape(uint32_t cntCirclePoints);
    ~SphereShape() = default;

protected:
    void FillVertex(VertexBufferRange<VertexPNC>& vb) const override;
    void FillIndex(IndexBufferRange<uint32_t>& ib, uint32_t vertexStartIndex) const override;

private:
    uint32_t m_cntCirclePoints;
};
