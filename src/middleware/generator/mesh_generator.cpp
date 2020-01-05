#include "middleware/generator/mesh_generator.h"

#include "core/math/constants.h"
#include "core/scene/geometry_node.h"


struct VertexPNC {
	dg::float3 position;
	dg::float3 normal;
	dg::float2 uv;

    // static const VertexDecl vDecl;
};

enum class Axis : uint8_t {
    X = 0,
    Y = 1,
    Z = 2,
};

std::shared_ptr<GeometryNode> MeshGenerator::CreateSolidCube(DevicePtr& device) {
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

std::shared_ptr<GeometryNode> MeshGenerator::CreateSolidSphere(DevicePtr& device, uint32_t cntCirclePoints) {
    cntCirclePoints = std::max(cntCirclePoints, uint32_t(3));
    uint32_t plg = cntCirclePoints / 2 - 1;

    uint32_t vertexCount = plg * cntCirclePoints + uint32_t(2);
    VertexBufferBuilder vbBuilder;
    auto vb = vbBuilder.AddRange<VertexPNC>(vertexCount);

    float angleB = -HalfPI<float>();
    float stepB = PI<float>() / static_cast<float>(plg + 1);
    float stepA = TwoPI<float>() / static_cast<float>(cntCirclePoints - 1);

    uint32_t ind = 1;
    for(auto ix=0; ix!=plg; ++ix) {
        angleB += stepB;
        float posY = std::sin(angleB) * 0.5f;
        float radius = std::cos(angleB) * 0.5f;
        float textureV = 0.5 - posY;

        float angleA = 0.0f;
        for(auto iy=0; iy!=cntCirclePoints; iy++) {
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
    uint32_t lastInd = vertexCount-1;
    vb[firstInd] = VertexPNC{dg::float3(0.0f,-0.5f,0.0f), dg::float3(0.0f,-1.0f,0.0f), dg::float2(0.5f,1.0f)};
    vb[lastInd]	 = VertexPNC{dg::float3(0.0f, 0.5f,0.0f), dg::float3(0.0f, 1.0f,0.0f), dg::float2(0.5f,0.0f)};

    uint32_t indexCount = 6*(cntCirclePoints-1)*plg;
    IndexBufferBuilder ibBuilder;
    auto ib = ibBuilder.AddRange<uint32_t>(indexCount);

    ind=0;
    for(uint32_t ix=0; ix!=plg-1; ++ix) {
        uint32_t z1,z2,z3,z4;
        z1 = ix*cntCirclePoints+1; z2 = z1+1;
        z3 = z1+cntCirclePoints;   z4 = z2+cntCirclePoints;
        for(auto iy=0; iy!=cntCirclePoints-1; iy++) {
            ib[ind++]=z1;	ib[ind++]=z3;	ib[ind++]=z4;
            ib[ind++]=z1;	ib[ind++]=z4;	ib[ind++]=z2;
            z1++; z2++; z3++; z4++;
        }
    }

    uint32_t iy = cntCirclePoints*(plg-1);
    for(uint32_t ix=1; ix!=cntCirclePoints; ix++) {
        ib[ind++]=ix;       ib[ind++]=ix+1;   ib[ind++]=firstInd;
        ib[ind++]=iy+ix+1;  ib[ind++]=iy+ix;  ib[ind++]=lastInd;
    }

    return std::make_shared<GeometryNode>(vbBuilder.Build(device, "sphere vb"), vb.OffsetBytes(),
        ibBuilder.Build(device, "sphere ib"), ib.OffsetBytes(), ib.Count(), ib.IsUint32());
}

std::shared_ptr<GeometryNode> MeshGenerator::CreateSolidCylinder(DevicePtr& device, uint32_t cntCirclePoints) {
	cntCirclePoints = std::max(cntCirclePoints, uint32_t(3));

    uint32_t vertexCount = 2 * cntCirclePoints;
    VertexBufferBuilder vbBuilder;
    auto vb = vbBuilder.AddRange<VertexPNC>(vertexCount);

	float angle = 0.f;
	float step = TwoPI<float>() / static_cast<float>(cntCirclePoints - 1);
	for(uint32_t i=0; i!=cntCirclePoints; ++i) {
        float posX = std::cos(angle) * 0.5f;
        float posZ = std::sin(angle) * 0.5f;
		angle+=step;

        dg::float3 normal = dg::normalize(dg::float3(posX, 0, posZ));
        float textureU = static_cast<float>(i) / static_cast<float>(cntCirclePoints - 1);

		// bottom
        uint32_t ind = i;
		vb[ind].position = dg::float3(posX, -0.5f, posZ);
		vb[ind].normal = normal;
		vb[ind].uv = dg::float2(textureU, 1);

		// top
        ind += cntCirclePoints;
		vb[ind].position = dg::float3(posX, 0.5f, posZ);
		vb[ind].normal = normal;
		vb[ind].uv = dg::float2(textureU, 0);
	}

    uint32_t indexCount = (cntCirclePoints - 1) * 6;
    IndexBufferBuilder ibBuilder;
    auto ib = ibBuilder.AddRange<uint32_t>(indexCount);

    uint32_t ind = 0;
    for(uint32_t i=0; i!=(cntCirclePoints - 1); ++i) {
        uint32_t bottomLeftVertex = i;
        uint32_t topLeftVertex = bottomLeftVertex + cntCirclePoints;

        ib[ind++] = bottomLeftVertex;
        ib[ind++] = bottomLeftVertex + 1;
        ib[ind++] = topLeftVertex;

        ib[ind++] = topLeftVertex;
        ib[ind++] = bottomLeftVertex + 1;
        ib[ind++] = topLeftVertex + 1;
    }

    return std::make_shared<GeometryNode>(vbBuilder.Build(device, "cylinder vb"), vb.OffsetBytes(),
        ibBuilder.Build(device, "cylinder ib"), ib.OffsetBytes(), ib.Count(), ib.IsUint32());
}

static std::shared_ptr<GeometryNode> CreateSolidPlane(DevicePtr& device, uint32_t cntWidthPoints, uint32_t cntHeightPoints, float scaleTextureWidth, float scaleTextureHeight, Axis axisUp) {
    cntWidthPoints = std::max(cntWidthPoints, uint32_t(2));
    cntHeightPoints = std::max(cntHeightPoints, uint32_t(2));

    uint32_t vertexCount = cntWidthPoints * cntHeightPoints;
    VertexBufferBuilder vbBuilder;
    auto vb = vbBuilder.AddRange<VertexPNC>(vertexCount);

    uint32_t ind = 0;
    for(uint32_t i=0; i!=cntWidthPoints; ++i) {
        float tu = static_cast<float>(i) / static_cast<float>(cntWidthPoints - 1);
        for(uint32_t j=0; j!=cntHeightPoints; ++j) {
            float tv = 1.f - static_cast<float>(j) / static_cast<float>(cntHeightPoints - 1);
            switch (axisUp) {
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
            vb[ind].uv = dg::float2(scaleTextureWidth * tu, scaleTextureHeight * tv);
            ++ind;
        }
    }

    uint32_t indexCount = (cntWidthPoints - 1) * (cntHeightPoints - 1) * 6;
    IndexBufferBuilder ibBuilder;
    auto ib = ibBuilder.AddRange<uint32_t>(indexCount);

    ind = 0;
    for(uint32_t i=0; i!=(cntWidthPoints - 1); ++i) {
        for(uint32_t j=0; j!=(cntHeightPoints - 1); ++j) {
            uint32_t bottomLeftVertex = i * cntWidthPoints + j;
            uint32_t topLeftVertex = bottomLeftVertex + cntHeightPoints;

            ib[ind++] = bottomLeftVertex;
            ib[ind++] = bottomLeftVertex + 1;
            ib[ind++] = topLeftVertex;

            ib[ind++] = topLeftVertex;
            ib[ind++] = bottomLeftVertex + 1;
            ib[ind++] = topLeftVertex + 1;
        }
    }

    return std::make_shared<GeometryNode>(vbBuilder.Build(device, "plane vb"), vb.OffsetBytes(),
        ibBuilder.Build(device, "plane ib"), ib.OffsetBytes(), ib.Count(), ib.IsUint32());
}

std::shared_ptr<GeometryNode> MeshGenerator::CreateSolidPlaneXZ(DevicePtr& device, uint32_t cntXPoints, uint32_t cntZPoints, float scaleTextureX, float scaleTextureZ) {
    return CreateSolidPlane(device, cntXPoints, cntZPoints, scaleTextureX, scaleTextureZ, Axis::Y);
}

std::shared_ptr<GeometryNode> MeshGenerator::CreateSolidPlaneXY(DevicePtr& device, uint32_t cntXPoints, uint32_t cntYPoints, float scaleTextureX, float scaleTextureY) {
    return CreateSolidPlane(device, cntXPoints, cntYPoints, scaleTextureX, scaleTextureY, Axis::Z);
}

std::shared_ptr<GeometryNode> MeshGenerator::CreateSolidPlaneZY(DevicePtr& device, uint32_t cntZPoints, uint32_t cntYPoints, float scaleTextureZ, float scaleTextureY) {
    return CreateSolidPlane(device, cntZPoints, cntYPoints, scaleTextureZ, scaleTextureY, Axis::X);
}
