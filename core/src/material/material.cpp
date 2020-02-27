#include "core/material/material.h"

#include "core/dg/device_context.h"
#include "core/dg/pipeline_state.h"


Material::Material(const PipelineStatePtr& pipelineState)
    : m_pipelineState(pipelineState) {

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
