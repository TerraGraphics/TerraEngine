#pragma once

#include "middleware/generator/shape.h"


class CylinderShape {
public:
    CylinderShape() = delete;
    CylinderShape(uint32_t cntCirclePoints);
    ~CylinderShape() = default;

    VertexBufferRange<VertexPNC> FillVertex(VertexBufferBuilder& vbBuilder);
    IndexBufferRange<uint32_t> FillIndex(IndexBufferBuilder& ibBuilder);

private:
    uint32_t m_cntCirclePoints;
};
