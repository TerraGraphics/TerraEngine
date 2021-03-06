#include "core/material/material_view.h"

#include "dg/context.h"
#include "core/common/exception.h"
#include "dg/shader_resource_binding.h"
#include "dg/shader_resource_variable.h"


struct MaterialView::Impl {
    Impl(const char* name, PipelineStatePtr pipelineState, ShaderResourceBindingPtr binding);

    const char* m_name = nullptr;
    PipelineStatePtr m_pipelineState;
    ShaderResourceBindingPtr m_binding;
};

MaterialView::Impl::Impl(const char* name, PipelineStatePtr pipelineState, ShaderResourceBindingPtr binding)
    : m_name(name)
    , m_pipelineState(pipelineState)
    , m_binding(binding) {

}

MaterialView::MaterialView(const char* name, PipelineStatePtr pipelineState, ShaderResourceBindingPtr binding)
    : impl(name, pipelineState, binding) {

}

MaterialView::MaterialView(const MaterialView& other)
    : impl(other.impl) {

}

MaterialView::~MaterialView() {

}

void MaterialView::SetVertexShaderVar(const char* name, DeviceRaw value) {
    if (auto var = impl->m_binding->GetVariableByName(dg::SHADER_TYPE_VERTEX, name); var != nullptr) {
        var->Set(value);
        return;
    }

    throw EngineError("Material:{}: unable to find variable '{}' for vertex shader", impl->m_name, name);
}

void MaterialView::SetPixelShaderVar(const char* name, DeviceRaw value) {
    if (auto var = impl->m_binding->GetVariableByName(dg::SHADER_TYPE_PIXEL, name); var != nullptr) {
        var->Set(value);
        return;
    }

    throw EngineError("Material:{}: unable to find variable '{}' for pixel shader", impl->m_name, name);
}

void MaterialView::SetGeometryShaderVar(const char* name, DeviceRaw value) {
    if (auto var = impl->m_binding->GetVariableByName(dg::SHADER_TYPE_GEOMETRY, name); var != nullptr) {
        var->Set(value);
        return;
    }

    throw EngineError("Material:{}: unable to find variable '{}' for geometry shader", impl->m_name, name);
}

void MaterialView::Bind(ContextPtr& context) {
    context->SetPipelineState(impl->m_pipelineState);
    context->CommitShaderResources(impl->m_binding, dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
}
