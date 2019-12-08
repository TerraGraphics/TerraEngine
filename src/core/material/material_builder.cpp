#include "core/material/material_builder.h"

#include <DiligentCore/Graphics/GraphicsEngine/interface/SwapChain.h>
#include <DiligentCore/Graphics/GraphicsEngine/interface/RenderDevice.h>
#include <DiligentCore/Graphics/GraphicsAccessories/interface/GraphicsAccessories.h>


bool MaterialBuilder::Builder::ShaderResourceVariableDescKey::operator<(const MaterialBuilder::Builder::ShaderResourceVariableDescKey& other) const noexcept {
    return (std::tie(shaderType, name) < std::tie(other.shaderType, other.name));
}

MaterialBuilder::Builder::StaticSamplerDesc::StaticSamplerDesc(dg::SHADER_TYPE shaderType, const dg::String& name, const dg::SamplerDesc& desc)
    : shaderType(shaderType)
    , name(name)
    , desc(desc) {

}

MaterialBuilder::Builder::Builder(MaterialBuilder* builder,
    dg::RefCntAutoPtr<dg::IShader>& shaderVS,
    dg::RefCntAutoPtr<dg::IShader>& shaderPS,
    const dg::InputLayoutDesc& layoutDesc)
    : m_builder(builder) {

    m_desc.IsComputePipeline = false;
    m_desc.GraphicsPipeline.NumRenderTargets = 1;
    m_desc.GraphicsPipeline.PrimitiveTopology = dg::PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    m_desc.GraphicsPipeline.DepthStencilDesc.DepthEnable = true;

    m_desc.GraphicsPipeline.pVS = shaderVS;
    m_desc.GraphicsPipeline.pPS = shaderPS;
    m_desc.GraphicsPipeline.InputLayout = layoutDesc;
    m_desc.GraphicsPipeline.RasterizerDesc.CullMode = dg::CULL_MODE_BACK;

}

MaterialBuilder::Builder& MaterialBuilder::Builder::CullMode(dg::CULL_MODE value) noexcept {
    m_desc.GraphicsPipeline.RasterizerDesc.CullMode = value;

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

MaterialBuilder::MaterialBuilder(const DevicePtr& device, const dg::RefCntAutoPtr<dg::ISwapChain>& swapChain)
    : m_device(device)
    , m_swapChain(swapChain) {

}

MaterialBuilder::Builder MaterialBuilder::Create(dg::RefCntAutoPtr<dg::IShader>& shaderVS, dg::RefCntAutoPtr<dg::IShader>& shaderPS, const dg::InputLayoutDesc& layoutDesc) {
    return Builder(this, shaderVS, shaderPS, layoutDesc);
}

std::shared_ptr<Material> MaterialBuilder::Build(dg::PipelineStateDesc& desc) {
    desc.GraphicsPipeline.RTVFormats[0] = m_swapChain->GetDesc().ColorBufferFormat;
    desc.GraphicsPipeline.DSVFormat = m_swapChain->GetDesc().DepthBufferFormat;

    dg::RefCntAutoPtr<dg::IPipelineState> pipelineState;
    m_device->CreatePipelineState(desc, &pipelineState);

    return std::make_shared<Material>(pipelineState);
}
