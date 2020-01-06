#include "middleware/generator/sphere_shape.h"

#include "core/common/exception.h"


SphereShape::SphereShape(uint32_t cntCirclePoints)
    : Shape((cntCirclePoints / 2 - 1) * cntCirclePoints + uint32_t(2), 6 * (cntCirclePoints - 1) * (cntCirclePoints / 2 - 1))
    , m_cntCirclePoints(cntCirclePoints) {

    if (m_cntCirclePoints < 3) {
        throw EngineError("Minimum value for cntCirclePoints in SphereShape is 3");
    }
}

void SphereShape::FillVertex(VertexBufferRange<VertexPNC>& vb) const {
    uint32_t plg = m_cntCirclePoints / 2 - 1;

    float angleB = -HalfPI<float>();
    float stepB = PI<float>() / static_cast<float>(plg + 1);
    float stepA = TwoPI<float>() / static_cast<float>(m_cntCirclePoints - 1);

    uint32_t ind = 1;
    for(auto ix=0; ix!=plg; ++ix) {
        angleB += stepB;
        float posY = std::sin(angleB) * 0.5f;
        float radius = std::cos(angleB) * 0.5f;
        float textureV = 0.5 - posY;

        float angleA = 0.0f;
        for(auto iy=0; iy!=m_cntCirclePoints; iy++) {
            float posX = std::cos(angleA) * radius;
            float posZ = std::sin(angleA) * radius;
            float textureU = angleA * 0.5f * OneOverPI<float>();

            vb[ind].position = dg::float3(posX, posY, posZ);
            vb[ind].normal = dg::normalize(vb[ind].position);
            vb[ind].uv = dg::float2(textureU, textureV);

            ++ind;
            angleA+=stepA;
        }
    }

    uint32_t firstInd = 0;
    uint32_t lastInd = m_vertexCount-1;
    vb[firstInd] = VertexPNC{dg::float3(0.0f,-0.5f,0.0f), dg::float3(0.0f,-1.0f,0.0f), dg::float2(0.5f,1.0f)};
    vb[lastInd]	 = VertexPNC{dg::float3(0.0f, 0.5f,0.0f), dg::float3(0.0f, 1.0f,0.0f), dg::float2(0.5f,0.0f)};
}

void SphereShape::FillIndex(IndexBufferRange<uint32_t>& ib, uint32_t vertexStartIndex) const {
    uint32_t plg = m_cntCirclePoints / 2 - 1;

    uint32_t ind = 0;
    for(uint32_t ix=0; ix!=plg-1; ++ix) {
        uint32_t bottomLeftVertex = ix * m_cntCirclePoints + 1 + vertexStartIndex;
        uint32_t topLeftVertex = bottomLeftVertex + m_cntCirclePoints;

        for(auto iy=0; iy!=m_cntCirclePoints-1; iy++) {
            ib[ind++] = bottomLeftVertex;
            ib[ind++] = topLeftVertex;
            ib[ind++] = topLeftVertex + 1;

            ib[ind++] = bottomLeftVertex;
            ib[ind++] = topLeftVertex + 1;
            ib[ind++] = bottomLeftVertex + 1;

            ++bottomLeftVertex;
            ++topLeftVertex;
        }
    }

    uint32_t firstInd = vertexStartIndex;
    uint32_t lastInd = vertexStartIndex + m_vertexCount - 1;
    uint32_t topVertexIndexOffset = m_cntCirclePoints * (plg - 1) + vertexStartIndex;
    for(uint32_t ix=1; ix!=m_cntCirclePoints; ix++) {
        uint32_t bottomVertexIndex = vertexStartIndex + ix;
        ib[ind++] = bottomVertexIndex;
        ib[ind++] = bottomVertexIndex + 1;
        ib[ind++] = firstInd;

        uint32_t topVertexIndex = topVertexIndexOffset + ix;
        ib[ind++] = topVertexIndex + 1;
        ib[ind++] = topVertexIndex;
        ib[ind++] = lastInd;
    }
}
