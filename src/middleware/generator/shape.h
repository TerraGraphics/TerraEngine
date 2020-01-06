#pragma once

#include <DiligentCore/Common/interface/BasicMath.h>

#include "core/scene/index_buffer.h"
#include "core/scene/vertex_buffer.h"


enum class Axis : uint8_t {
    X = 0,
    Y = 1,
    Z = 2,
};

struct VertexPNC {
	dg::float3 position;
	dg::float3 normal;
	dg::float2 uv;
};
