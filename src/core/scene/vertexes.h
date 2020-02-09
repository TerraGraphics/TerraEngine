#pragma once

#include "core/material/vertex_decl.h"


struct VertexP {
	dg::float3 position;

    static const VertexDecl& GetDecl();
};

struct VertexPNC {
	dg::float3 position;
	dg::float3 normal;
	dg::float2 uv;

    static const VertexDecl& GetDecl();
};
