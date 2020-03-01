#include "core/scene/vertexes.h"

#include <memory>
#include "core/material/vertex_decl.h"


const VertexDecl& VertexP::GetDecl() {
    static const auto desc = VertexDecl({ItemDecl<dg::float3>("position")});

    return desc;
}

const VertexDecl& VertexPNC::GetDecl() {
    static const auto desc = VertexDecl({ItemDecl<dg::float3>("position"), ItemDecl<dg::float3>("normal"), ItemDecl<dg::float2>("uv")});

    return desc;
}