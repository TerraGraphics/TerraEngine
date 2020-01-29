#pragma once

#include <DiligentCore/Common/interface/BasicMath.h>

#include "core/math/basic.h"
#include "core/math/constants.h"
#include "core/scene/index_buffer.h"
#include "core/scene/vertex_buffer.h"


enum class Axis : uint8_t {
    X = 0,
    Y = 1,
    Z = 2,
};

struct VertexPNC {
	dg::float3 position;
	dg::float3 normal;
	dg::float2 uv;
};

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

    using VertexCallback = std::function<void (const dg::float2&, VertexPNC&)>;
    void FlatGenerator(VertexBufferRange<VertexPNC>& vb, const UInt2& segments, const VertexCallback& callback) const;
    void FlatGenerator(VertexBufferRange<VertexPNC>& vb, const UInt2& segments, bool downVertexGlued, bool upVertexGlued, const VertexCallback& callback) const;
    void FlatGenerator(IndexBufferRange<uint32_t>& ib, const UInt2& segments, uint32_t vertexStartIndex) const;
    void FlatGenerator(IndexBufferRange<uint32_t>& ib, const UInt2& segments, bool downVertexGlued, bool upVertexGlued, uint32_t vertexStartIndex) const;

    static dg::float2 ToDXTexCoord(const dg::float2& coord);

protected:
    dg::float4x4 m_matrix = dg::One4x4;
    dg::float2 m_texScale = dg::float2(1.f, 1.f);
    bool m_matrixChanged = false;

    uint32_t m_vertexCount = 0;
    uint32_t m_indexCount = 0;
};
