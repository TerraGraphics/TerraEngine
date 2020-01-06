#pragma once

#include "middleware/generator/shape.h"


class CubeShape {
public:
    CubeShape() = default;
    ~CubeShape() = default;

    VertexBufferRange<VertexPNC> FillVertex(VertexBufferBuilder& vbBuilder);
    IndexBufferRange<uint32_t> FillIndex(IndexBufferBuilder& ibBuilder);
};
