#include "middleware/generator/cube_shape.h"


VertexBufferRange<VertexPNC> CubeShape::FillVertex(VertexBufferBuilder& vbBuilder) {
    uint32_t vertexCount = 24;
    auto vb = vbBuilder.AddRange<VertexPNC>(vertexCount);

    vb[ 0].position	= dg::float3(-0.5f,-0.5f,-0.5f);
    vb[ 1].position	= dg::float3(-0.5f, 0.5f,-0.5f);
    vb[ 2].position	= dg::float3( 0.5f, 0.5f,-0.5f);
    vb[ 3].position	= dg::float3( 0.5f,-0.5f,-0.5f);

    vb[ 4].position	= dg::float3( 0.5f,-0.5f, 0.5f);
    vb[ 5].position	= dg::float3( 0.5f, 0.5f, 0.5f);
    vb[ 6].position	= dg::float3(-0.5f, 0.5f, 0.5f);
    vb[ 7].position	= dg::float3(-0.5f,-0.5f, 0.5f);

    vb[ 8].position	= dg::float3(-0.5f,-0.5f, 0.5f);
    vb[ 9].position	= dg::float3(-0.5f, 0.5f, 0.5f);
    vb[10].position	= dg::float3(-0.5f, 0.5f,-0.5f);
    vb[11].position	= dg::float3(-0.5f,-0.5f,-0.5f);

    vb[12].position	= dg::float3( 0.5f,-0.5f,-0.5f);
    vb[13].position	= dg::float3( 0.5f, 0.5f,-0.5f);
    vb[14].position	= dg::float3( 0.5f, 0.5f, 0.5f);
    vb[15].position	= dg::float3( 0.5f,-0.5f, 0.5f);

    vb[16].position	= dg::float3(-0.5f,-0.5f, 0.5f);
    vb[17].position	= dg::float3(-0.5f,-0.5f,-0.5f);
    vb[18].position	= dg::float3( 0.5f,-0.5f,-0.5f);
    vb[19].position	= dg::float3( 0.5f,-0.5f, 0.5f);

    vb[20].position	= dg::float3(-0.5f, 0.5f,-0.5f);
    vb[21].position	= dg::float3(-0.5f, 0.5f, 0.5f);
    vb[22].position	= dg::float3( 0.5f, 0.5f, 0.5f);
    vb[23].position	= dg::float3( 0.5f, 0.5f,-0.5f);

    uint32_t ind = 0;
    for(uint32_t i=0; i!=6; ++i) {
        vb[ind++].uv = dg::float2(0.0f,1.0f);
        vb[ind++].uv = dg::float2(0.0f,0.0f);
        vb[ind++].uv = dg::float2(1.0f,0.0f);
        vb[ind++].uv = dg::float2(1.0f,1.0f);
    }

    for(uint32_t i=0; i!=8; ++i) {
        float axis = (i < 4) ? -1.0f : 1.0f;
        vb[i+ 0].normal = dg::float3(0.0f, 0.0f, axis);
        vb[i+ 8].normal = dg::float3(axis, 0.0f, 0.0f);
        vb[i+16].normal = dg::float3(0.0f, axis, 0.0f);
    }

    return std::move(vb);
}

IndexBufferRange<uint32_t> CubeShape::FillIndex(IndexBufferBuilder& ibBuilder) {
    uint32_t indexCount = 12 * 3;
    auto ib = ibBuilder.AddRange<uint32_t>(indexCount);

    uint32_t ind = 0;
    for(uint32_t i=0; i!=6; ++i) {
        uint32_t offset = i * 4;
        ib[ind++] = offset; ib[ind++] = offset + 1; ib[ind++] = offset + 2;
        ib[ind++] = offset; ib[ind++] = offset + 2; ib[ind++] = offset + 3;
    }

    return std::move(ib);
}
