#include "middleware/generator/mesh_generator.h"

#include "core/math/constants.h"
#include "core/scene/geometry_node.h"


struct VertexPNC {
	dg::float3 position;
	dg::float3 normal;
	dg::float2 uv;

    // static const VertexDecl vDecl;
};

std::shared_ptr<GeometryNode> MeshGenerator::CreateSolidCube(DevicePtr device) {
    uint32_t vertexCount = 24;
    VertexBufferBuilder vbBuilder;
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

    uint32_t indexCount = 12 * 3;
    IndexBufferBuilder ibBuilder;
    auto ib = ibBuilder.AddRange<uint32_t>(indexCount);

    ind = 0;
    for(uint32_t i=0; i!=6; ++i) {
        uint32_t offset = i * 4;
        ib[ind++] = offset; ib[ind++] = offset + 1; ib[ind++] = offset + 2;
        ib[ind++] = offset; ib[ind++] = offset + 2; ib[ind++] = offset + 3;
    }

    return std::make_shared<GeometryNode>(vbBuilder.Build(device, "cube vb"), vb.OffsetBytes(),
        ibBuilder.Build(device, "cube ib"), ib.OffsetBytes(), ib.Count(), ib.IsUint32());
}

std::shared_ptr<GeometryNode> MeshGenerator::CreateSolidSphere(DevicePtr device, uint16_t cntVertexCircle) {
    uint16_t plg = cntVertexCircle / 2 - 1;

    uint32_t vertexCount = static_cast<uint32_t>(plg) * static_cast<uint32_t>(cntVertexCircle) + uint32_t(2);
    VertexBufferBuilder vbBuilder;
    auto vb = vbBuilder.AddRange<VertexPNC>(vertexCount);

    float angleB = -HalfPI<float>();
    float stepB = PI<float>() / static_cast<float>(plg + 1);
    float stepA = TwoPI<float>() / static_cast<float>(cntVertexCircle - 1);

    uint32_t ind = 1;
    for(auto ix=0; ix!=plg; ++ix) {
        angleB += stepB;
        float posY = std::sin(angleB) * 0.5f;
        float radius = std::cos(angleB) * 0.5f;
        float textureV = 0.f - posY;

        float angleA = 0.0f;
        for(auto iy=0; iy!=cntVertexCircle; iy++) {
            float posZ = std::sin(angleA) * radius;
            float posX = std::cos(angleA) * radius;
            float textureU = angleA * 0.5f * OneOverPI<float>();

            vb[ind].position = dg::float3(posX, posY, posZ);
            vb[ind].normal = dg::normalize(vb[ind].position);
            vb[ind].uv = dg::float2(textureU, textureV);

            ++ind;
            angleA+=stepA;
        }
    }

    uint32_t firstInd = 0;
    uint32_t lastInd = vertexCount-1;
    vb[firstInd] = VertexPNC{dg::float3(0.0f,-0.5f,0.0f), dg::float3(0.0f,-1.0f,0.0f), dg::float2(0.5f,1.0f)};
    vb[lastInd]	 = VertexPNC{dg::float3(0.0f, 0.5f,0.0f), dg::float3(0.0f, 1.0f,0.0f), dg::float2(0.5f,0.0f)};

    uint32_t indexCount = 6*(cntVertexCircle-1)*plg;
    IndexBufferBuilder ibBuilder;
    auto ib = ibBuilder.AddRange<uint32_t>(indexCount);

    ind=0;
    for(uint32_t ix=0; ix!=plg-1; ++ix) {
        uint32_t z1,z2,z3,z4;
        z1 = ix*cntVertexCircle+1; z2 = z1+1;
        z3 = z1+cntVertexCircle;   z4 = z2+cntVertexCircle;
        for(auto iy=0; iy!=cntVertexCircle-1; iy++) {
            ib[ind++]=z1;	ib[ind++]=z3;	ib[ind++]=z4;
            ib[ind++]=z1;	ib[ind++]=z4;	ib[ind++]=z2;
            z1++; z2++; z3++; z4++;
        }
    }

    uint32_t iy = cntVertexCircle*(plg-1);
    for(uint32_t ix=1; ix!=cntVertexCircle; ix++) {
        ib[ind++]=ix;       ib[ind++]=ix+1;   ib[ind++]=firstInd;
        ib[ind++]=iy+ix+1;  ib[ind++]=iy+ix;  ib[ind++]=lastInd;
    }

    return std::make_shared<GeometryNode>(vbBuilder.Build(device, "sphere vb"), vb.OffsetBytes(),
        ibBuilder.Build(device, "sphere ib"), ib.OffsetBytes(), ib.Count(), ib.IsUint32());
}

std::shared_ptr<GeometryNode> MeshGenerator::CreateSolidCylinder(DevicePtr device, uint16_t cntVertexCircle) {
	cntVertexCircle = std::max(cntVertexCircle, uint16_t(3));

    uint32_t vertexCount = 4 * cntVertexCircle;
    VertexBufferBuilder vbBuilder;
    auto vb = vbBuilder.AddRange<VertexPNC>(vertexCount);

	float angle = 0.f;
	float step = TwoPI<float>() / static_cast<float>(cntVertexCircle);
	for(uint32_t i=0; i!=cntVertexCircle; ++i) {
        float s = std::sin(angle);
        float c = std::cos(angle);
		angle+=step;

		dg::float3 pos(c*0.5f, -0.5f, s*0.5f);
		dg::float3 norm = dg::normalize(dg::float3(pos.x, 0, pos.z));
		dg::float2 tex(pos.x + 0.5f, pos.z + 0.5f);

		float tu = static_cast<float>(i)/static_cast<float>(cntVertexCircle-1);

		// bottom circle
		vb[i].position	= pos;
		vb[i].normal	= dg::float3(0,-1,0);
		vb[i].uv	= tex;
		// bottom side
		vb[i+cntVertexCircle*1].position = pos;
		vb[i+cntVertexCircle*1].normal   = norm;
		vb[i+cntVertexCircle*1].uv       = dg::float2(tu,0);
		// top side
		pos.y = -pos.y;
		vb[i+cntVertexCircle*2].position = pos;
		vb[i+cntVertexCircle*2].normal   = norm;
		vb[i+cntVertexCircle*2].uv       = dg::float2(tu,1);
		// top circle
		vb[i+cntVertexCircle*3].position = pos;
		vb[i+cntVertexCircle*3].normal   = dg::float3(0, 1, 0);
		vb[i+cntVertexCircle*3].uv       = tex;
	}

    uint32_t indexCount = (cntVertexCircle - 1) * 12;
    IndexBufferBuilder ibBuilder;
    auto ib = ibBuilder.AddRange<uint32_t>(indexCount);

	uint32_t num = 0;
	uint32_t addSm = (cntVertexCircle - 2)*3;
	uint32_t addV = cntVertexCircle*3;

    //  bottom + top circle
	for(uint32_t i=2; i!=cntVertexCircle; ++i) {
		ib[num+0] = 0;
		ib[num+1] = i-1;
		ib[num+2] = i;

		ib[num+0+addSm] = addV;
		ib[num+1+addSm] = addV+i-1;
		ib[num+2+addSm] = addV+i;
		num+=3;
	}

	// bottom + top side
	num = addSm*2;
	for(uint32_t i=0; i!=uint32_t(cntVertexCircle); ++i) {
		uint32_t z1 = cntVertexCircle + i;
		uint32_t z2 = cntVertexCircle + (i+1)%cntVertexCircle;
		uint32_t z3 = cntVertexCircle + z1;
		uint32_t z4 = cntVertexCircle + z2;
		ib[num++] = z1;
		ib[num++] = z3;
		ib[num++] = z4;
		ib[num++] = z1;
		ib[num++] = z4;
		ib[num++] = z2;
	}

    return std::make_shared<GeometryNode>(vbBuilder.Build(device, "cylinder vb"), vb.OffsetBytes(),
        ibBuilder.Build(device, "cylinder ib"), ib.OffsetBytes(), ib.Count(), ib.IsUint32());
}

std::shared_ptr<GeometryNode> MeshGenerator::CreateSolidPlane(DevicePtr device, uint32_t cntXSides, uint32_t cntZSides, float scaleTextureX, float scaleTextureZ) {
    cntXSides = std::max(cntXSides, uint32_t(2));
    cntZSides = std::max(cntZSides, uint32_t(2));

    uint32_t vertexCount = (cntXSides+1)*(cntZSides+1);
    VertexBufferBuilder vbBuilder;
    auto vb = vbBuilder.AddRange<VertexPNC>(vertexCount);

    uint32_t ind = 0;
    for(uint32_t i=0; i!=cntXSides + 1; ++i) {
        float tu = static_cast<float>(i)/static_cast<float>(cntXSides);
        for(uint32_t j=0; j!=cntZSides + 1; ++j) {
            float tv = static_cast<float>(j)/static_cast<float>(cntZSides);
            vb[ind].position = dg::float3(tu-0.5f, 0.0f, tv-0.5f);
            vb[ind].normal = dg::float3(0.0f,    1.0f, 0.0f);
            vb[ind].uv = dg::float2(scaleTextureX*tu, scaleTextureZ*tv);
            ++ind;
        }
    }

    uint32_t indexCount = cntXSides*cntZSides*6;
    IndexBufferBuilder ibBuilder;
    auto ib = ibBuilder.AddRange<uint32_t>(indexCount);

    ind = 0;
    for(uint32_t i=0; i!=cntXSides; ++i) {
        for(uint32_t j=0; j!=cntZSides; ++j) {
            uint32_t z1 = i * static_cast<uint32_t>(cntXSides + 1) + j;
            uint32_t z2 = z1 + static_cast<uint32_t>(cntZSides + 1);
            ib[ind++] = z1;
            ib[ind++] = z1+1;
            ib[ind++] = z2;
            ib[ind++] = z2;
            ib[ind++] = z1+1;
            ib[ind++] = z2+1;
        }
    }

    return std::make_shared<GeometryNode>(vbBuilder.Build(device, "cylinder vb"), vb.OffsetBytes(),
        ibBuilder.Build(device, "cylinder ib"), ib.OffsetBytes(), ib.Count(), ib.IsUint32());
}
