#pragma once

#include <DiligentCore/Common/interface/BasicMath.h>

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

protected:
    virtual void FillVertex(VertexBufferRange<VertexPNC>& vb) const = 0;
    virtual void FillIndex(IndexBufferRange<uint32_t>& ib, uint32_t vertexStartIndex) const = 0;

protected:
    // TODO: generate normal matrix
    dg::float4x4 m_matrix = dg::One4x4;
    bool m_matrixChanged = false;

    uint32_t m_vertexCount = 0;
    uint32_t m_indexCount = 0;
};
