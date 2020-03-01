#pragma once

#include <cstdint>
#include <functional>
#include "core/math/basic.h"
#include "core/math/constants.h"
#include "core/scene/index_buffer.h"


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

    static dg::float2 ToDXTexCoord(const dg::float2& coord);

protected:
    dg::float4x4 m_matrix = dg::One4x4;
    dg::float2 m_texScale = dg::float2(1.f, 1.f);
    bool m_matrixChanged = false;

    uint32_t m_vertexCount = 0;
    uint32_t m_indexCount = 0;
};

class FlatPlaneGenerator : public Shape {
protected:
    FlatPlaneGenerator(const math::UInt2& segments);

    using VertexCallback = std::function<void (const dg::float2&, VertexPNC&)>;
    void Generate(VertexBufferRange<VertexPNC>& vb, const VertexCallback& callback) const;
    void FillIndex(IndexBufferRange<uint32_t>& ib, uint32_t vertexStartIndex) const final;

private:
    math::UInt2 m_segments;
};