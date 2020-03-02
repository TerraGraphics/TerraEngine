#include "middleware/generator/shape.h"

 #include "core/scene/vertexes.h"
 #include "core/scene/vertex_buffer.h"


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

dg::float2 Shape::ToDXTexCoord(const dg::float2& coord) {
    return dg::float2(coord.u, 1.f - coord.v);
}

FlatPlaneGenerator::FlatPlaneGenerator(const math::UInt2& segments)
    : Shape((segments.x + 1) * (segments.y + 1), segments.x * segments.y * 6)
    , m_generator(segments) {

}

void FlatPlaneGenerator::Generate(VertexBufferRange<VertexPNC>& vb, UVGridGenerator::Callback&& callback) const {
    m_generator.SetCallback(std::move(callback));

    uint32_t ind = 0;
    for(auto&& v: m_generator.GetVertexes()) {
        vb[ind++] = std::move(v);
    }
}

void FlatPlaneGenerator::FillIndex(IndexBufferRange<uint32_t>& ib, uint32_t vertexStartIndex) const {
    uint32_t ind = 0;
    for(auto index: m_generator.GetIndexes(vertexStartIndex)) {
        ib[ind++] = index;
    }
}
