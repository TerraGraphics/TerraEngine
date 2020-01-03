#include "middleware/generator/mesh_generator.h"

#include "core/scene/geometry_node.h"


struct VertexPNC {
	dg::float3 position;
	dg::float3 normal;
	dg::float2 uv;

    // static const VertexDecl vDecl;
};

std::shared_ptr<GeometryNode> MeshGenerator::CreateSolidCube(DevicePtr device) {
    VertexPNC vb[24];
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

    for(int i=0,j=0; i<6; ++i) {
        vb[j++].uv = dg::float2(0.0f,1.0f);
        vb[j++].uv = dg::float2(0.0f,0.0f);
        vb[j++].uv = dg::float2(1.0f,0.0f);
        vb[j++].uv = dg::float2(1.0f,1.0f);
    }

    for(int i=0; i<8; ++i) {
        float zn = (i<4) ? -1.0f : 1.0f;
        vb[i+ 0].normal = dg::float3(0.0f, 0.0f,  zn);
        vb[i+ 8].normal = dg::float3(zn,   0.0f, 0.0f);
        vb[i+16].normal = dg::float3(0.0f, zn,   0.0f);
    }

    size_t j = 0;
    uint32_t ib[12 * 3];
    for(uint32_t i=0; i!=6; ++i) {
        uint32_t sm = i * 4;
        ib[j++] = sm; ib[j++] = sm + 1; ib[j++] = sm + 2;
        ib[j++] = sm; ib[j++] = sm + 2; ib[j++] = sm + 3;
    }

    return std::make_shared<GeometryNode>(
        VertexBuffer(device, vb, sizeof(vb), "Cube vertex buffer"),
        IndexBuffer(device, ib, sizeof(ib), "Cube index buffer"));
}

std::shared_ptr<GeometryNode> MeshGenerator::CreateSolidSphere(DevicePtr device, uint16_t cntVertexCircle) {
    cntVertexCircle = std::min(cntVertexCircle, uint16_t(363));
    uint16_t plg = cntVertexCircle/2 - 1;

    // CAABB box;
    float B = -dg::PI_F * 0.5f;
    float stepB = dg::PI_F / float(plg + 1);
    float stepA = 2.0f * dg::PI_F / float(cntVertexCircle - 1);

    uint32_t ind = 1;
    uint16_t vertexCnt = plg*cntVertexCircle + 2;
    auto* vb = new VertexPNC[vertexCnt];
    for(auto ix=0; ix!=plg; ++ix) {
        B += stepB;
        float Vy = std::sin(B);
        float rad = std::cos(B);
        float tv = (1.0f - Vy) / 2.0f;

        float A = 0.0f;
        for(auto iy=0; iy!=cntVertexCircle; iy++) {
            float s = std::sin(A);
            float c = std::cos(A);

            vb[ind].position = dg::float3(rad * c, Vy, rad * s) * 0.5f;
            vb[ind].uv = dg::float2(A * 0.5f / dg::PI_F, tv);
            vb[ind].normal   = dg::normalize(vb[ind].position);
            ind++;
            A+=stepA;
        }
    }

    vb[0]			= VertexPNC{dg::float3(0.0f,-0.5f,0.0f), dg::float3(0.0f,-1.0f,0.0f), dg::float2(0.5f,1.0f)};
    vb[vertexCnt-1]	= VertexPNC{dg::float3(0.0f, 0.5f,0.0f), dg::float3(0.0f, 1.0f,0.0f), dg::float2(0.5f,0.0f)};


    ind=0;
    size_t indexCnt = 6*(cntVertexCircle-1)*plg;
    uint32_t* ib = new uint32_t[indexCnt];
    for(uint32_t ix=0; ix!=plg-1; ++ix) {
        uint32_t z1,z2,z3,z4;
        z1=ix*cntVertexCircle+1;	z2=z1+1;
        z3=z1+cntVertexCircle;		z4=z2+cntVertexCircle;
        for(auto iy=0; iy!=cntVertexCircle-1; iy++) {
            ib[ind++]=z1;	ib[ind++]=z3;	ib[ind++]=z4;
            ib[ind++]=z1;	ib[ind++]=z4;	ib[ind++]=z2;
            z1++; z2++; z3++; z4++;
        }
    }

    uint32_t iy = cntVertexCircle*(plg-1);
    for(uint32_t ix=1; ix!=cntVertexCircle; ix++) {
        ib[ind++]=ix;       ib[ind++]=ix+1;   ib[ind++]=0;
        ib[ind++]=iy+ix+1;  ib[ind++]=iy+ix;  ib[ind++]=vertexCnt-1;
    }


    auto node = std::make_shared<GeometryNode>(
        VertexBuffer(device, vb, vertexCnt * sizeof(VertexPNC), "Sphere vertex buffer"),
        IndexBuffer(device, ib, indexCnt * sizeof(*ib), "Sphere index buffer"));
    delete []vb;
    delete []ib;

    return node;
}

std::shared_ptr<GeometryNode> MeshGenerator::CreateSolidCylinder(DevicePtr device, uint16_t cntVertexCircle) {
	cntVertexCircle = std::max(cntVertexCircle, uint16_t(3));
	uint32_t vertexCnt = 4*cntVertexCircle;

	float angle = 0;
	float step = 2.0f * dg::PI_F/float(cntVertexCircle);
    auto* vb = new VertexPNC[vertexCnt];
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

	uint32_t num = 0;
	uint32_t addSm = (cntVertexCircle - 2)*3;
	uint32_t addV = cntVertexCircle*3;
    size_t indexCnt = (cntVertexCircle - 1) * 12;
    uint32_t* ib = new uint32_t[indexCnt];

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


    auto node = std::make_shared<GeometryNode>(
        VertexBuffer(device, vb, vertexCnt * sizeof(VertexPNC), "Cylinder vertex buffer"),
        IndexBuffer(device, ib, indexCnt * sizeof(*ib), "Cylinder index buffer"));
    delete []vb;
    delete []ib;

    return node;
}

std::shared_ptr<GeometryNode> MeshGenerator::CreateSolidPlane(DevicePtr device, uint32_t cntXSides, uint32_t cntZSides, float scaleTextureX, float scaleTextureZ) {
    cntXSides = std::max(cntXSides, uint32_t(2));
    cntZSides = std::max(cntZSides, uint32_t(2));

    uint32_t ind = 0;
    uint32_t vertexCnt = (cntXSides+1)*(cntZSides+1);
    auto* vb = new VertexPNC[vertexCnt];
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


    ind = 0;
    uint32_t indexCnt = cntXSides*cntZSides*6;
    uint32_t* ib = new uint32_t[indexCnt];
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

    auto node = std::make_shared<GeometryNode>(
        VertexBuffer(device, vb, vertexCnt * sizeof(VertexPNC), "Cylinder vertex buffer"),
        IndexBuffer(device, ib, indexCnt * sizeof(*ib), "Cylinder index buffer"));
    delete []vb;
    delete []ib;

    return node;
}
