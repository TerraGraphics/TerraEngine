#include "core/material/material_builder.h"

#include <vector>
#include <utility>
#include <unordered_map>

#include <DiligentCore/Graphics/GraphicsEngine/interface/SwapChain.h>
#include <DiligentCore/Graphics/GraphicsEngine/interface/InputLayout.h>
#include <DiligentCore/Graphics/GraphicsEngine/interface/ShaderResourceVariable.h>

#include "core/dg/device.h"
#include "core/dg/sampler.h"
#include "core/common/hash.h"
#include "core/common/exception.h"
#include "core/dg/graphics_types.h"
#include "core/material/vdecl_storage.h"
#include "core/material/shader_builder.h"
#include "core/material/microshader_loader.h"


namespace {

struct ShaderVar {
    std::string name;
    dg::SHADER_TYPE shaderType;
    uint16_t samplerId;
    dg::SHADER_RESOURCE_VARIABLE_TYPE type;

    bool operator==(const ShaderVar& other) const {
        return ((type == other.type) && (samplerId == other.samplerId) && (shaderType == other.shaderType) && (name == other.name));
    }
};

}

namespace std {
    template<> struct hash<dg::SamplerDesc> {
        size_t operator()(const dg::SamplerDesc& value) const {
            size_t hash = 0;
            HashCombine(hash, value.MinFilter);
            HashCombine(hash, value.MagFilter);
            HashCombine(hash, value.MipFilter);
            HashCombine(hash, value.AddressU);
            HashCombine(hash, value.AddressV);
            HashCombine(hash, value.AddressW);
            HashCombine(hash, value.MipLODBias);
            HashCombine(hash, value.MaxAnisotropy);
            HashCombine(hash, value.ComparisonFunc);
            HashCombine(hash, value.BorderColor[0]);
            HashCombine(hash, value.BorderColor[1]);
            HashCombine(hash, value.BorderColor[2]);
            HashCombine(hash, value.BorderColor[3]);
            HashCombine(hash, value.MinLOD);
            HashCombine(hash, value.MaxLOD);

            return hash;
        }
    };

    template<> struct hash<ShaderVar> {
        size_t operator()(const ShaderVar& value) const {
            auto hash = std::hash<std::string>()(value.name);
            auto fieldsSum = (
                (static_cast<uint64_t>(value.shaderType) << uint64_t(32)) |
                (static_cast<uint64_t>(value.samplerId) << uint64_t(8)) |
                static_cast<uint64_t>(value.type));
            HashCombine(hash, fieldsSum);

            return hash;
        }
    };
}

struct MaterialBuilder::Impl {
    Impl();

    // valid id more than 0
    const ShaderVar& GetShaderVar(uint16_t textureVarId) const;
    const dg::SamplerDesc& GetSamplerDesc(uint16_t id) const;
    uint16_t CacheShaderVar(const std::string& name, dg::SHADER_TYPE shaderType, dg::SHADER_RESOURCE_VARIABLE_TYPE type, uint16_t samplerId);
    uint16_t CacheShaderVar(uint16_t textureVarId, const dg::SamplerDesc& desc);
    void FillVars(const ShaderVars& vars, dg::PipelineResourceLayoutDesc& desc);

    uint16_t m_defaultSamplerId;
    std::vector<dg::SamplerDesc> m_idToSampler;
    std::unordered_map<dg::SamplerDesc, uint16_t> m_samplerToId;
    std::vector<ShaderVar> m_idToShaderVar;
    std::unordered_map<ShaderVar, uint16_t> m_shaderVarToId;

    dg::ShaderResourceVariableDesc m_varsDescs[ShaderVars::max];
    dg::StaticSamplerDesc m_samplers[ShaderVars::max];
};

MaterialBuilder::Impl::Impl() {
    dg::SamplerDesc desc;
    desc.AddressU = dg::TEXTURE_ADDRESS_WRAP;
    desc.AddressV = dg::TEXTURE_ADDRESS_WRAP;
    desc.AddressW = dg::TEXTURE_ADDRESS_WRAP;

    m_defaultSamplerId = static_cast<uint16_t>(m_idToShaderVar.size() + 1);
    m_samplerToId[desc] = m_defaultSamplerId;
    m_idToSampler.push_back(desc);
}

const ShaderVar& MaterialBuilder::Impl::GetShaderVar(uint16_t textureVarId) const {
    if (textureVarId == 0) {
        throw EngineError("MaterialBuilder: wrong textureVarId {} for GetShaderVar, min textureVarId is 1", textureVarId);
    }

    if (textureVarId > m_idToShaderVar.size()) {
        throw EngineError("MaterialBuilder: wrong textureVarId {} for GetShaderVar, max textureVarId is {}", textureVarId, m_idToShaderVar.size());
    }

    return m_idToShaderVar[textureVarId - 1];
}

const dg::SamplerDesc& MaterialBuilder::Impl::GetSamplerDesc(uint16_t samplerId) const {
    if (samplerId == 0) {
        throw EngineError("MaterialBuilder: wrong samplerId {} for GetSamplerDesc, min samplerId is 1", samplerId);
    }
    if (samplerId > m_idToSampler.size()) {
        throw EngineError("MaterialBuilder: wrong samplerId {} for GetSamplerDesc, max samplerId is {}", samplerId, m_idToSampler.size());
    }

    return m_idToSampler[samplerId - 1];
}

uint16_t MaterialBuilder::Impl::CacheShaderVar(const std::string& name, dg::SHADER_TYPE shaderType, dg::SHADER_RESOURCE_VARIABLE_TYPE type, uint16_t samplerId) {
    ShaderVar value{name, shaderType, samplerId, type};
    if (const auto it = m_shaderVarToId.find(value); it != m_shaderVarToId.cend()) {
        return it->second;
    }

    auto id = static_cast<uint16_t>(m_idToShaderVar.size() + 1);
    m_shaderVarToId[value] = id;
    m_idToShaderVar.push_back(value);

    return id;
}

uint16_t MaterialBuilder::Impl::CacheShaderVar(uint16_t textureVarId, const dg::SamplerDesc& desc) {
    uint16_t samplerId;
    if (const auto it = m_samplerToId.find(desc); it != m_samplerToId.cend()) {
        samplerId = it->second;
    } else {
        samplerId = static_cast<uint16_t>(m_idToSampler.size() + 1);
        m_samplerToId[desc] = samplerId;
        m_idToSampler.push_back(desc);
    }

    auto& var = GetShaderVar(textureVarId);
    return CacheShaderVar(var.name, var.shaderType, var.type, samplerId);
}

void MaterialBuilder::Impl::FillVars(const ShaderVars& vars, dg::PipelineResourceLayoutDesc& desc) {
    desc.DefaultVariableType = dg::SHADER_RESOURCE_VARIABLE_TYPE_STATIC;
    desc.Variables = m_varsDescs;
    desc.NumVariables = static_cast<uint32_t>(vars.number);
    desc.StaticSamplers = m_samplers;
    desc.NumStaticSamplers = 0;
    for(uint8_t i=0; i!=vars.number; ++i) {
        const auto& shaderVar = GetShaderVar(vars.vars[i]);
        m_varsDescs[i] = {shaderVar.shaderType, shaderVar.name.c_str(), shaderVar.type};
        if (shaderVar.samplerId != 0) {
            const auto& samplerDesc = GetSamplerDesc(shaderVar.samplerId);
            m_samplers[desc.NumStaticSamplers++] = {shaderVar.shaderType, shaderVar.name.c_str(), samplerDesc};
        }
    }
}

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

uint16_t MaterialBuilder::CacheShaderVar(const std::string& name, dg::SHADER_TYPE shaderType, dg::SHADER_RESOURCE_VARIABLE_TYPE type) {
    return impl->CacheShaderVar(name, shaderType, type, 0);
}

uint16_t MaterialBuilder::CacheTextureVar(const std::string& name, dg::SHADER_TYPE shaderType, dg::SHADER_RESOURCE_VARIABLE_TYPE type) {
    return impl->CacheShaderVar(name, shaderType, type, impl->m_defaultSamplerId);
}

uint16_t MaterialBuilder::CacheTextureVar(uint16_t textureVarId, const dg::SamplerDesc& desc) {
    return impl->CacheShaderVar(textureVarId, desc);
}

const dg::SamplerDesc& MaterialBuilder::GetCachedSamplerDesc(uint16_t textureVarId) const {
    return impl->GetSamplerDesc(impl->GetShaderVar(textureVarId).samplerId);
}

PipelineStatePtr MaterialBuilder::Create(uint64_t mask, uint16_t vDeclIdPerVertex, uint16_t vDeclIdPerInstance, const ShaderVars& vars, dg::GraphicsPipelineDesc& gpDesc) {
    auto vDeclId =  m_vDeclStorage->Join(vDeclIdPerVertex, vDeclIdPerInstance);
    auto src = m_microShaderLoader->GetSources(mask, m_vDeclStorage->GetSemanticDecls(vDeclId));
    const auto& layoutElements = m_vDeclStorage->GetLayoutElements(vDeclId);
    auto shaders = m_shaderBuilder->Build(src);

    dg::PipelineStateDesc desc;
    desc.IsComputePipeline = false;

    gpDesc.NumRenderTargets = src.gsOutputNumber;
    for (uint8_t i=0; i!=src.gsOutputNumber; ++i) {
        gpDesc.RTVFormats[i] = m_swapChain->GetDesc().ColorBufferFormat;
    }
    gpDesc.DSVFormat = m_swapChain->GetDesc().DepthBufferFormat;
    gpDesc.pVS = shaders.vs;
    gpDesc.pPS = shaders.ps;
    gpDesc.pGS = shaders.gs;
    gpDesc.InputLayout = dg::InputLayoutDesc(layoutElements.data(), static_cast<uint32_t>(layoutElements.size()));
    desc.GraphicsPipeline = gpDesc;
    impl->FillVars(vars, desc.ResourceLayout);

    PipelineStatePtr pipelineState;
    dg::PipelineStateCreateInfo createInfo { desc, dg::PSO_CREATE_FLAG_NONE };
    m_device->CreatePipelineState(createInfo, &pipelineState);
    m_staticVarsStorage->SetVars(pipelineState);

    return pipelineState;
}
