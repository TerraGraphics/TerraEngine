#pragma once

#include <DiligentCore/Common/interface/BasicMath.h>
#include <DiligentCore/Graphics/GraphicsEngine/interface/InputLayout.h>

#include "core/common/dg.h"


struct VertexP {
	dg::float3 position;

    static const dg::InputLayoutDesc& GetStandartLayoutDesc();
};

struct VertexPNC {
	dg::float3 position;
	dg::float3 normal;
	dg::float2 uv;

    static const dg::InputLayoutDesc& GetStandartLayoutDesc();
};
