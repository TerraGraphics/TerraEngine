#pragma once

#include "middleware/generator/shape.h"


class CubeShape : public Shape {
public:
    CubeShape();
    ~CubeShape() = default;

protected:
    void FillVertex(VertexBufferRange<VertexPNC>& vb) const override;
    void FillIndex(IndexBufferRange<uint32_t>& ib, uint32_t vertexStartIndex) const override;
};
