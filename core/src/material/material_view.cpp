#include "core/material/material_view.h"

#include <DiligentCore/Graphics/GraphicsEngine/interface/Shader.h>
#include <DiligentCore/Graphics/GraphicsEngine/interface/ShaderResourceVariable.h>
#include <DiligentCore/Graphics/GraphicsEngine/interface/ShaderResourceBinding.h>

#include "core/dg/context.h"
#include "core/common/exception.h"
#include "core/material/material_new.h"


struct MaterialView::Impl {
    Impl(const std::string& name, const std::shared_ptr<MaterialNew>& material);

    std::string m_name;
    PipelineStatePtr m_pipelineState;
    ShaderResourceBindingPtr m_binding;
    std::shared_ptr<MaterialNew> m_material;
};

MaterialView::Impl::Impl(const std::string& name, const std::shared_ptr<MaterialNew>& material)
    : m_name(name)
    , m_material(material) {

}

MaterialView::MaterialView(const std::string& name, const std::shared_ptr<MaterialNew>& material)
    : impl(name, material) {

}

MaterialView::~MaterialView() {

}

void MaterialView::UpdatePipeline(PipelineStatePtr pipelineState, ShaderResourceBindingPtr binding) {
    impl->m_pipelineState = pipelineState;
    impl->m_binding = binding;
}

void MaterialView::SetVertexShaderVar(const char* name, DeviceRaw value) {
    if (auto var = impl->m_binding->GetVariableByName(dg::SHADER_TYPE_VERTEX, name); var != nullptr) {
        var->Set(value);
    }

    throw EngineError("unable to find variable '{}' in material {} for vertex shader", name, impl->m_name);
}

void MaterialView::SetPixelShaderVar(const char* name, DeviceRaw value) {
    if (auto var = impl->m_binding->GetVariableByName(dg::SHADER_TYPE_PIXEL, name); var != nullptr) {
        var->Set(value);
    }

    throw EngineError("unable to find variable '{}' in material {} for pixel shader", name, impl->m_name);
}

void MaterialView::SetGeometryShaderVar(const char* name, DeviceRaw value) {
    if (auto var = impl->m_binding->GetVariableByName(dg::SHADER_TYPE_GEOMETRY, name); var != nullptr) {
        var->Set(value);
    }

    throw EngineError("unable to find variable '{}' in material {} for geometry shader", name, impl->m_name);
}

void MaterialView::Update(DevicePtr& device, ContextPtr& context) {
    impl->m_material->Update(device, context, this);
}

void MaterialView::Bind(ContextPtr& context) {
    context->SetPipelineState(impl->m_pipelineState);
    context->CommitShaderResources(impl->m_binding, dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
}
