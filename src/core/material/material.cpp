#include "core/material/material.h"

#include <DiligentCore/Graphics/GraphicsEngine/interface/DeviceContext.h>


Material::Material(const dg::RefCntAutoPtr<dg::IPipelineState>& pipelineState)
    : m_pipelineState(pipelineState) {

}

void Material::Bind(ContextPtr& context) {
    context->SetPipelineState(m_pipelineState);
}
