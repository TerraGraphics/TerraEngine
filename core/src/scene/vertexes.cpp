#include "core/scene/vertexes.h"

#include <memory>

#include "core/engine.h"
#include "core/material/vdecl_item.h"
#include "core/material/vdecl_storage.h"


uint16_t VertexP::GetVDeclId() {
    static auto vDeclId = Engine::Get().GetVDeclStorage()->Add({VDeclItem("position", VDeclType::Float3)});

    return vDeclId;
}

uint16_t VertexPNC::GetVDeclId() {
    static auto vDeclId = Engine::Get().GetVDeclStorage()->Add({
        VDeclItem("position", VDeclType::Float3),
        VDeclItem("normal", VDeclType::Float3),
        VDeclItem("uv", VDeclType::Float2)});

    return vDeclId;
}
