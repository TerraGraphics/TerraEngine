#pragma once

#include "middleware/generator/shape.h"


class SphereShape {
public:
    SphereShape() = delete;
    SphereShape(uint32_t cntCirclePoints);
    ~SphereShape() = default;

    VertexBufferRange<VertexPNC> FillVertex(VertexBufferBuilder& vbBuilder);
    IndexBufferRange<uint32_t> FillIndex(IndexBufferBuilder& ibBuilder);

private:
    uint32_t m_cntCirclePoints;
};
