#include "core/material/material_builder.h"

#include <fmt/format.h>
#include <DiligentCore/Graphics/GraphicsEngine/interface/SwapChain.h>
#include <DiligentCore/Graphics/GraphicsEngine/interface/RenderDevice.h>
#include <DiligentCore/Graphics/GraphicsAccessories/interface/GraphicsAccessories.h>

#include "core/material/shader_builder.h"
#include "core/material/microshader_loader.h"
#include "core/material/microshader_loader_old.h"


bool MaterialBuilder::Builder::ShaderResourceVariableDescKey::operator<(const MaterialBuilder::Builder::ShaderResourceVariableDescKey& other) const noexcept {
    return (std::tie(shaderType, name) < std::tie(other.shaderType, other.name));
}

MaterialBuilder::Builder::StaticSamplerDesc::StaticSamplerDesc(dg::SHADER_TYPE shaderType, const dg::String& name, const dg::SamplerDesc& desc)
    : shaderType(shaderType)
    , name(name)
    , desc(desc) {

}

MaterialBuilder::Builder::Builder(MaterialBuilder* builder, ShaderPtr& shaderVS, ShaderPtr& shaderPS, ShaderPtr& shaderGS, const dg::InputLayoutDesc& layoutDesc)
    : m_builder(builder) {

    m_desc.IsComputePipeline = false;
    m_desc.GraphicsPipeline.NumRenderTargets = 1;
    m_desc.GraphicsPipeline.PrimitiveTopology = dg::PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    m_desc.GraphicsPipeline.DepthStencilDesc.DepthEnable = true;

    m_desc.GraphicsPipeline.pVS = shaderVS;
    m_desc.GraphicsPipeline.pPS = shaderPS;
    m_desc.GraphicsPipeline.pGS = shaderGS;
    m_desc.GraphicsPipeline.InputLayout = layoutDesc;
    m_desc.GraphicsPipeline.RasterizerDesc.CullMode = dg::CULL_MODE_BACK;

}

MaterialBuilder::Builder& MaterialBuilder::Builder::DepthEnable(bool value) noexcept {
    m_desc.GraphicsPipeline.DepthStencilDesc.DepthEnable = value;

    return *this;
}

MaterialBuilder::Builder& MaterialBuilder::Builder::CullMode(dg::CULL_MODE value) noexcept {
    m_desc.GraphicsPipeline.RasterizerDesc.CullMode = value;

    return *this;
}

MaterialBuilder::Builder& MaterialBuilder::Builder::Topology(dg::PRIMITIVE_TOPOLOGY value) noexcept {
    m_desc.GraphicsPipeline.PrimitiveTopology = value;

    return *this;
}

MaterialBuilder::Builder& MaterialBuilder::Builder::Var(dg::SHADER_TYPE shaderType, const dg::String& name, dg::SHADER_RESOURCE_VARIABLE_TYPE type) noexcept {
    ShaderResourceVariableDescKey key{shaderType, name};
    if (m_vars.find(key) != m_vars.cend()) {
        LOG_ERROR_AND_THROW("Shader varaible ", name, " is duplicated for shader type ", dg::GetShaderTypeLiteralName(shaderType));
    }
    m_vars[key] = type;

    return *this;
}

MaterialBuilder::Builder& MaterialBuilder::Builder::TextureVar(dg::SHADER_TYPE shaderType, const dg::String& name, const dg::SamplerDesc& desc, dg::SHADER_RESOURCE_VARIABLE_TYPE type) noexcept {
    Var(shaderType, name, type);
    m_samplers.emplace_back(shaderType, name, desc);

    return *this;
}

MaterialBuilder::Builder& MaterialBuilder::Builder::TextureVar(dg::SHADER_TYPE shaderType, const dg::String& name, dg::TEXTURE_ADDRESS_MODE addressMode, dg::SHADER_RESOURCE_VARIABLE_TYPE type) noexcept {
    dg::SamplerDesc desc;
    desc.AddressU = addressMode;
    desc.AddressV = addressMode;
    desc.AddressW = addressMode;

    Var(shaderType, name, type);
    m_samplers.emplace_back(shaderType, name, desc);

    return *this;
}

std::shared_ptr<Material> MaterialBuilder::Builder::Build(const dg::Char* name) {
    m_desc.Name = (name != nullptr) ? name : "no name is assigned";
    m_desc.ResourceLayout.DefaultVariableType = dg::SHADER_RESOURCE_VARIABLE_TYPE_STATIC;


    std::unique_ptr<dg::ShaderResourceVariableDesc[]> vars(new dg::ShaderResourceVariableDesc[m_vars.size()]);
    auto* varsIt = vars.get();
    for (const auto& [key, varType]: m_vars) {
        *varsIt = {key.shaderType, key.name.c_str(), varType};
        ++varsIt;
    }

    m_desc.ResourceLayout.Variables = vars.get();
    m_desc.ResourceLayout.NumVariables = m_vars.size();


    std::unique_ptr<dg::StaticSamplerDesc[]> samplers;
    if (!m_samplers.empty()) {
        samplers = std::unique_ptr<dg::StaticSamplerDesc[]> (new dg::StaticSamplerDesc[m_samplers.size()]);
        auto* samplersIt = samplers.get();
        for (const auto& sampler: m_samplers) {
            *samplersIt = {sampler.shaderType, sampler.name.c_str(), sampler.desc};
            ++samplersIt;
        }

        m_desc.ResourceLayout.StaticSamplers = samplers.get();
    }
    m_desc.ResourceLayout.NumStaticSamplers = m_samplers.size();

    return m_builder->Build(m_desc);
}

MaterialBuilder::MaterialBuilder(const DevicePtr& device, const ContextPtr& context, const SwapChainPtr& swapChain, const EngineFactoryPtr& engineFactory)
    : m_device(device)
    , m_swapChain(swapChain)
    , m_shaderBuilder(new ShaderBuilder(device, engineFactory))
    , m_staticVarsStorage(new StaticVarsStorage(device, context))
    , m_microShaderLoader(new MicroshaderLoader())
    , m_microShaderLoaderOld(new MicroshaderLoaderOld()) {

}

MaterialBuilder::~MaterialBuilder() {
    if (m_microShaderLoader) {
        delete m_microShaderLoader;
        m_microShaderLoader = nullptr;
    }
    if (m_microShaderLoaderOld) {
        delete m_microShaderLoaderOld;
        m_microShaderLoaderOld = nullptr;
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
    return m_microShaderLoaderOld->GetMask(name);
}

void MaterialBuilder::Load(const MaterialBuilderDesc& desc) {
    m_microShaderLoader->Load(desc);
    m_microShaderLoaderOld->Load(desc);
    m_shaderBuilder->Create(desc);
}

MaterialBuilder::Builder MaterialBuilder::Create(uint64_t mask, const dg::InputLayoutDesc& layoutDesc) {
    auto src = m_microShaderLoader->GetSources(mask);
    auto srcOld = m_microShaderLoaderOld->GetSources(mask);
    auto shaders = m_shaderBuilder->Build(srcOld);

    return Builder(this, shaders.vs, shaders.ps, shaders.gs, layoutDesc);
}

std::shared_ptr<Material> MaterialBuilder::Build(dg::PipelineStateDesc& desc) {
    desc.GraphicsPipeline.RTVFormats[0] = m_swapChain->GetDesc().ColorBufferFormat;
    desc.GraphicsPipeline.DSVFormat = m_swapChain->GetDesc().DepthBufferFormat;
    // desc.GraphicsPipeline.RasterizerDesc.FillMode = dg::FILL_MODE_WIREFRAME;

    PipelineStatePtr pipelineState;
    m_device->CreatePipelineState(desc, &pipelineState);
    m_staticVarsStorage->SetVars(pipelineState);

    return std::make_shared<Material>(pipelineState);
}
