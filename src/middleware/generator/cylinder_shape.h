#pragma once

#include "middleware/generator/shape.h"


class CylinderShape : public Shape {
public:
    CylinderShape() = delete;
    CylinderShape(uint32_t cntCirclePoints);
    ~CylinderShape() = default;

protected:
    void FillVertex(VertexBufferRange<VertexPNC>& vb) const override;
    void FillIndex(IndexBufferRange<uint32_t>& ib, uint32_t vertexStartIndex) const override;

private:
    uint32_t m_cntCirclePoints;
};
