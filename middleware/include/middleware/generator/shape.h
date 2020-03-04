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
protected:
    Shape() = delete;
    Shape(uint32_t vertexCount, uint32_t indexCount);
    ~Shape() = default;

public:
    void SetTranform(const dg::float4x4& matrix);
    void SetTexScale(const dg::float2& value);

protected:
    virtual void FillVertex(VertexBufferRange<VertexPNC>& vb) const = 0;
    virtual void FillIndex(IndexBufferRange<uint32_t>& ib, uint32_t vertexStartIndex) const = 0;

protected:
    dg::float4x4 m_matrix = dg::One4x4;
    dg::float2 m_texScale = dg::float2(1.f, 1.f);
    bool m_matrixChanged = false;

    uint32_t m_vertexCount = 0;
    uint32_t m_indexCount = 0;
};

class FlatPlaneGenerator : public Shape {
protected:
    FlatPlaneGenerator(const std::string& name, const math::UInt2& segments, const math::Axis3& orientation, const dg::float3& center);

    void FillVertex(VertexBufferRange<VertexPNC>& vb) const final;
    void FillIndex(IndexBufferRange<uint32_t>& ib, uint32_t vertexStartIndex) const final;

protected:
    UVGridGenerator m_generator;
    dg::float3 m_center;
};
