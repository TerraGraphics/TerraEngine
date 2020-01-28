#include "middleware/generator/shape.h"


Shape::Shape(uint32_t vertexCount, uint32_t indexCount)
    : m_vertexCount(vertexCount)
    , m_indexCount(indexCount) {

}

void Shape::SetTranform(const dg::float4x4& matrix) {
    m_matrix = matrix;
    m_matrixChanged = true;
}

void Shape::SetTexScale(const dg::float2& value) {
    m_texScale = value;
}

void Shape::FlatGenerator(VertexBufferRange<VertexPNC>& vb, const UInt2& segments, const VertexCallback& callback) const {
    uint32_t ind = 0;
    dg::float2 coord;
    for(uint32_t y=0; y!=(segments.y + 1); ++y) {
        coord.y = static_cast<float>(y) / static_cast<float>(segments.y);
        for(uint32_t x=0; x!=(segments.x + 1); ++x) {
            coord.x = static_cast<float>(x) / static_cast<float>(segments.x);
            callback(coord, vb[ind]);
            vb[ind].uv *= m_texScale;
            ++ind;
        }
    }
}

void Shape::FlatGenerator(IndexBufferRange<uint32_t>& ib, const UInt2& segments, uint32_t vertexStartIndex) const {
    uint32_t ind = 0;
    uint32_t offset = vertexStartIndex;
    for(uint32_t y=0; y!=segments.y; ++y) {
        for(uint32_t x=0; x!=segments.x; ++x) {
            uint32_t bottomLeftVertex = offset + x;
            uint32_t topLeftVertex = bottomLeftVertex + segments.x + 1;

            ib[ind++] = bottomLeftVertex;
            ib[ind++] = bottomLeftVertex + 1;
            ib[ind++] = topLeftVertex;

            ib[ind++] = topLeftVertex;
            ib[ind++] = bottomLeftVertex + 1;
            ib[ind++] = topLeftVertex + 1;
        }
        offset += segments.x + 1;
    }
}

dg::float2 Shape::ToDXTexCoord(const dg::float2& coord) {
    return dg::float2(coord.u, 1.f - coord.v);
}
