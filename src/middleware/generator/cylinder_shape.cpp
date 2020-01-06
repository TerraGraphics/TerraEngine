#include "middleware/generator/cylinder_shape.h"

#include "core/math/constants.h"


CylinderShape::CylinderShape(uint32_t cntCirclePoints)
    : m_cntCirclePoints(std::max(cntCirclePoints, uint32_t(3))) {

}

VertexBufferRange<VertexPNC> CylinderShape::FillVertex(VertexBufferBuilder& vbBuilder) {
    uint32_t vertexCount = 2 * m_cntCirclePoints;
    auto vb = vbBuilder.AddRange<VertexPNC>(vertexCount);

	float angle = 0.f;
	float step = TwoPI<float>() / static_cast<float>(m_cntCirclePoints - 1);
	for(uint32_t i=0; i!=m_cntCirclePoints; ++i) {
        float posX = std::cos(angle) * 0.5f;
        float posZ = std::sin(angle) * 0.5f;
		angle+=step;

        dg::float3 normal = dg::normalize(dg::float3(posX, 0, posZ));
        float textureU = static_cast<float>(i) / static_cast<float>(m_cntCirclePoints - 1);

		// bottom
        uint32_t ind = i;
		vb[ind].position = dg::float3(posX, -0.5f, posZ);
		vb[ind].normal = normal;
		vb[ind].uv = dg::float2(textureU, 1);

		// top
        ind += m_cntCirclePoints;
		vb[ind].position = dg::float3(posX, 0.5f, posZ);
		vb[ind].normal = normal;
		vb[ind].uv = dg::float2(textureU, 0);
	}

    return std::move(vb);
}

IndexBufferRange<uint32_t> CylinderShape::FillIndex(IndexBufferBuilder& ibBuilder) {
    uint32_t indexCount = (m_cntCirclePoints - 1) * 6;
    auto ib = ibBuilder.AddRange<uint32_t>(indexCount);

    uint32_t ind = 0;
    for(uint32_t i=0; i!=(m_cntCirclePoints - 1); ++i) {
        uint32_t bottomLeftVertex = i;
        uint32_t topLeftVertex = bottomLeftVertex + m_cntCirclePoints;

        ib[ind++] = bottomLeftVertex;
        ib[ind++] = bottomLeftVertex + 1;
        ib[ind++] = topLeftVertex;

        ib[ind++] = topLeftVertex;
        ib[ind++] = bottomLeftVertex + 1;
        ib[ind++] = topLeftVertex + 1;
    }

    return std::move(ib);
}
