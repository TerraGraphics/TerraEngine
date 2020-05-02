#include "core/scene/vertexes.h"

#include <memory>
#include "core/material/vertex_decl.h"


const VertexDecl& VertexP::GetDecl() {
    static const auto desc = VertexDecl({VDeclItem("position", VDeclType::Float3)});

    return desc;
}

const VertexDecl& VertexPNC::GetDecl() {
    static const auto desc = VertexDecl({
        VDeclItem("position", VDeclType::Float3),
        VDeclItem("normal", VDeclType::Float3),
        VDeclItem("uv", VDeclType::Float2)});

    return desc;
}
