#include "core/material/material.h"

#include "core/dg/context.h"


Material::Material(const PipelineStatePtr& pipelineState, uint32_t vDeclId)
    : m_vDeclId(vDeclId)
    , m_pipelineState(pipelineState) {

}

const char* Material::GetName() const {
    return m_pipelineState->GetDesc().Name;
}

ShaderResourceBindingPtr Material::CreateShaderResourceBinding() {
    ShaderResourceBindingPtr binding;
    m_pipelineState->CreateShaderResourceBinding(&binding, true);

    return binding;
}

void Material::Bind(ContextPtr& context) {
    context->SetPipelineState(m_pipelineState);
}
