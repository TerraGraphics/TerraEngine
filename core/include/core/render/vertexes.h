#pragma once

#include <cstdint>
#include "core/dg/math.h"


struct VertexP {
	dg::float3 position;

    static uint16_t GetVDeclId();
};

struct VertexPNC {
	dg::float3 position;
	dg::float3 normal;
	dg::float2 uv;

    static uint16_t GetVDeclId();
};
