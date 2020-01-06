#include "middleware/generator/plane_shape.h"


PlaneShape::PlaneShape(uint32_t cntWidthPoints, uint32_t cntHeightPoints, float scaleTextureWidth, float scaleTextureHeight, Axis axisUp)
    : m_cntWidthPoints(std::max(cntWidthPoints, uint32_t(2)))
    , m_cntHeightPoints(std::max(cntHeightPoints, uint32_t(2)))
    , m_scaleTextureWidth(scaleTextureWidth)
    , m_scaleTextureHeight(scaleTextureHeight)
    , m_axisUp(axisUp) {

}

VertexBufferRange<VertexPNC> PlaneShape::FillVertex(VertexBufferBuilder& vbBuilder) {
    uint32_t vertexCount = m_cntWidthPoints * m_cntHeightPoints;
    auto vb = vbBuilder.AddRange<VertexPNC>(vertexCount);

    uint32_t ind = 0;
    for(uint32_t i=0; i!=m_cntWidthPoints; ++i) {
        float tu = static_cast<float>(i) / static_cast<float>(m_cntWidthPoints - 1);
        for(uint32_t j=0; j!=m_cntHeightPoints; ++j) {
            float tv = 1.f - static_cast<float>(j) / static_cast<float>(m_cntHeightPoints - 1);
            switch (m_axisUp) {
                case Axis::X:
                    vb[ind].position = dg::float3(0.f, 0.5f - tv, tu - 0.5f);
                    vb[ind].normal = dg::float3(1.f, 0.f, 0.f);
                    break;
                case Axis::Y:
                    vb[ind].position = dg::float3(tu - 0.5f, 0.f, 0.5f - tv);
                    vb[ind].normal = dg::float3(0.f, 1.f, 0.f);
                    break;
                case Axis::Z:
                default:
                    vb[ind].position = dg::float3(tu - 0.5f, 0.5f - tv, 0.f);
                    vb[ind].normal = dg::float3(0.f, 0.f, 1.f);
                    break;
            }
            vb[ind].uv = dg::float2(m_scaleTextureWidth * tu, m_scaleTextureHeight * tv);
            ++ind;
        }
    }

    return std::move(vb);
}

IndexBufferRange<uint32_t> PlaneShape::FillIndex(IndexBufferBuilder& ibBuilder) {
    uint32_t indexCount = (m_cntWidthPoints - 1) * (m_cntHeightPoints - 1) * 6;
    auto ib = ibBuilder.AddRange<uint32_t>(indexCount);

    uint32_t ind = 0;
    for(uint32_t i=0; i!=(m_cntWidthPoints - 1); ++i) {
        for(uint32_t j=0; j!=(m_cntHeightPoints - 1); ++j) {
            uint32_t bottomLeftVertex = i * m_cntWidthPoints + j;
            uint32_t topLeftVertex = bottomLeftVertex + m_cntHeightPoints;

            ib[ind++] = bottomLeftVertex;
            ib[ind++] = bottomLeftVertex + 1;
            ib[ind++] = topLeftVertex;

            ib[ind++] = topLeftVertex;
            ib[ind++] = bottomLeftVertex + 1;
            ib[ind++] = topLeftVertex + 1;
        }
    }

    return std::move(ib);
}
