#pragma once

#include <cstdint>
#include <functional>
#include "core/math/basic.h"
#include "core/math/constants.h"
#include "core/scene/index_buffer.h"
#include "middleware/generator/uv_grid_generator.h"


struct VertexPNC;
template <typename Vertex> class VertexBufferRange;
class Shape {
    friend class ShapeBuilder;
public:
    void SetTranform(const dg::float4x4& matrix);
    void SetTexScale(const dg::float2& value);

    virtual const UVGridGenerator::Vertexes GetVertexes() const = 0;
    virtual const UVGridGenerator::Indexes GetIndexes(uint32_t vertexStartIndex) const = 0;

protected:
    dg::float4x4 m_matrix = dg::One4x4;
    dg::float2 m_texScale = dg::float2(1.f, 1.f);
    bool m_matrixChanged = false;
};
