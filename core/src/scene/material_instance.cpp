#include "core/scene/material_instance.h"

#include <DiligentCore/Graphics/GraphicsEngine/interface/Shader.h>
#include <DiligentCore/Graphics/GraphicsEngine/interface/ShaderResourceBinding.h>
#include <DiligentCore/Graphics/GraphicsEngine/interface/ShaderResourceVariable.h>

#include "core/dg/context.h"
#include "core/common/exception.h"
#include "core/material/material.h"


MaterialInstance::MaterialInstance(const std::shared_ptr<Material>& material)
    : m_material(material)
    , m_binding(material->CreateShaderResourceBinding()) {

}

MaterialInstance::~MaterialInstance() {

}

uint32_t MaterialInstance::GetVDeclId() const noexcept {
    return m_material->GetVDeclId();
}

void MaterialInstance::SetVertexShaderVar(const char* name, DeviceRaw value) {
    ShaderResourceVariableRaw var = m_binding->GetVariableByName(dg::SHADER_TYPE_VERTEX, name);
    if (!var) {
        throw EngineError("unable to find variable '{}' in material {} for vertex shader", name, m_material->GetName());
    }

    var->Set(value);
}

void MaterialInstance::SetPixelShaderVar(const char* name, DeviceRaw value) {
    ShaderResourceVariableRaw var = m_binding->GetVariableByName(dg::SHADER_TYPE_PIXEL, name);
    if (!var) {
        throw EngineError("unable to find variable '{}' in material {} for pixel shader", name, m_material->GetName());
    }

    var->Set(value);
}

void MaterialInstance::Bind(ContextPtr& context) {
    m_material->Bind(context);
    context->CommitShaderResources(m_binding, dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
}
