#include "core/material/material_builder.h"

#include <tuple>
#include <utility>
#include <type_traits>

#include <DiligentCore/Graphics/GraphicsEngine/interface/SwapChain.h>
#include <DiligentCore/Graphics/GraphicsEngine/interface/InputLayout.h>
#include <DiligentCore/Graphics/GraphicsEngine/interface/DepthStencilState.h>
#include <DiligentCore/Graphics/GraphicsEngine/interface/ShaderResourceVariable.h>

#include "core/dg/errors.h"
#include "core/dg/device.h"
#include "core/dg/rasterizer_state.h"
#include "core/material/vdecl_storage.h"
#include "core/material/shader_builder.h"
#include "core/dg/graphics_accessories.h"
#include "core/material/microshader_loader.h"


MaterialBuilder::MaterialBuilder(const DevicePtr& device, const ContextPtr& context,
    const SwapChainPtr& swapChain, const EngineFactoryPtr& engineFactory, const std::shared_ptr<VDeclStorage>& vDeclStorage)
    : m_device(device)
    , m_swapChain(swapChain)
    , m_vDeclStorage(vDeclStorage)
    , m_shaderBuilder(new ShaderBuilder(device, engineFactory))
    , m_microShaderLoader(new MicroshaderLoader())
    , m_staticVarsStorage(new StaticVarsStorage(device, context)) {

}

MaterialBuilder::~MaterialBuilder() {
    if (m_microShaderLoader) {
        delete m_microShaderLoader;
        m_microShaderLoader = nullptr;
    }
    if (m_staticVarsStorage) {
        delete m_staticVarsStorage;
        m_staticVarsStorage = nullptr;
    }
    if (m_shaderBuilder) {
        delete m_shaderBuilder;
        m_shaderBuilder = nullptr;
    }
}

uint64_t MaterialBuilder::GetShaderMask(const std::string& name) const {
    return m_microShaderLoader->GetMask(name);
}

void MaterialBuilder::Load(const MaterialBuilderDesc& desc) {
    m_microShaderLoader->Load(desc);
    m_shaderBuilder->Create(desc);
}

PipelineStatePtr MaterialBuilder::Create(uint64_t mask, uint16_t vDeclIdPerVertex, uint16_t vDeclIdPerInstance, dg::PipelineStateDesc& desc) {
    auto vDeclId =  m_vDeclStorage->Join(vDeclIdPerVertex, vDeclIdPerInstance);
    auto src = m_microShaderLoader->GetSources(mask, m_vDeclStorage->GetSemanticDecls(vDeclId));
    const auto& layoutElements = m_vDeclStorage->GetLayoutElements(vDeclId);
    auto shaders = m_shaderBuilder->Build(src);

    auto& gp = desc.GraphicsPipeline;
    gp.NumRenderTargets = src.gsOutputNumber;
    for (uint8_t i=0; i!=src.gsOutputNumber; ++i) {
        gp.RTVFormats[i] = m_swapChain->GetDesc().ColorBufferFormat;
    }
    gp.DSVFormat = m_swapChain->GetDesc().DepthBufferFormat;
    gp.pVS = shaders.vs;
    gp.pPS = shaders.ps;
    gp.pGS = shaders.gs;
    gp.InputLayout = dg::InputLayoutDesc(layoutElements.data(), static_cast<uint32_t>(layoutElements.size()));

    PipelineStatePtr pipelineState;
    dg::PipelineStateCreateInfo createInfo { desc, dg::PSO_CREATE_FLAG_NONE };
    m_device->CreatePipelineState(createInfo, &pipelineState);
    m_staticVarsStorage->SetVars(pipelineState);

    return pipelineState;
}
