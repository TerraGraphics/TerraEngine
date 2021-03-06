#include "core/material/material_builder.h"

#include <vector>
#include <utility>
#include <unordered_map>

#include "dg/device.h"
#include "dg/swap_chain.h" // IWYU pragma: keep
#include "core/common/hash.h"
#include "core/common/exception.h"
#include "dg/shader_resource_variable.h"
#include "core/material/material_vars.h"
#include "core/material/vdecl_storage.h"
#include "core/material/shader_builder.h"
#include "core/material/microshader_loader.h"


namespace {

bool operator==(const dg::GraphicsPipelineDesc& first, const dg::GraphicsPipelineDesc& second) {
    return (
        (first.DepthStencilDesc.DepthEnable == second.DepthStencilDesc.DepthEnable) &&
        (first.RasterizerDesc.CullMode == second.RasterizerDesc.CullMode) &&
        (first.RasterizerDesc.FrontCounterClockwise == second.RasterizerDesc.FrontCounterClockwise) &&
        (first.PrimitiveTopology == second.PrimitiveTopology));
}

struct ShaderVar {
    bool operator==(const ShaderVar& other) const {
        return ((type == other.type) && (samplerId == other.samplerId) && (shaderType == other.shaderType) && (name == other.name));
    }

    std::string name;
    dg::SHADER_TYPE shaderType;
    uint16_t samplerId;
    dg::SHADER_RESOURCE_VARIABLE_TYPE type;
};

struct PipelineStateKey {
    PipelineStateKey(uint64_t mask, uint16_t vDeclId, const ShaderVars& vars, const dg::GraphicsPipelineDesc& gpDesc)
        : mask(mask)
        , vDeclId(vDeclId)
        , vars(vars)
        , gpDesc(gpDesc) {

    }

    bool operator==(const PipelineStateKey& other) const {
        return ((mask == other.mask) && (vDeclId == other.vDeclId) && (vars == other.vars) && (gpDesc == other.gpDesc));
    }

    uint64_t mask;
    uint16_t vDeclId;
    ShaderVars vars;
    dg::GraphicsPipelineDesc gpDesc;
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

    template<> struct hash<TargetsFormat> {
        size_t operator()(const TargetsFormat& value) const {
            size_t hash = 0;
            HashCombine(hash, value.formats, value.countColorTargets + 1);

            return hash;
        }
    };

    template<> struct hash<PipelineStateKey> {
        size_t operator()(const PipelineStateKey& value) const {
            auto hash = std::hash<uint64_t>()(value.mask);
            HashCombine(hash, value.vDeclId);
            HashCombine(hash, value.vars.vars, static_cast<size_t>(value.vars.number));
            HashCombine(hash, value.gpDesc.DepthStencilDesc.DepthEnable);
            HashCombine(hash, value.gpDesc.RasterizerDesc.CullMode);
            HashCombine(hash, value.gpDesc.RasterizerDesc.FrontCounterClockwise);
            HashCombine(hash, value.gpDesc.PrimitiveTopology);

            return hash;
        }
    };
}

bool ShaderVars::operator==(const ShaderVars& other) const {
    if (number == other.number) {
        for (uint8_t i=0; i!=number; ++i) {
            if (vars[i] != other.vars[i]) {
                return false;
            }
        }

        return true;
    }

    return false;
}

bool TargetsFormat::operator==(const TargetsFormat& other) const {
    if (countColorTargets == other.countColorTargets) {
        for (uint8_t i=0; i!=(other.countColorTargets + 1); ++i) {
            if (formats[i] != other.formats[i]) {
                return false;
            }
        }

        return true;
    }

    return false;
}

dg::TEXTURE_FORMAT TargetsFormat::GetDepthTarget() const noexcept {
    return formats[0];
}

void TargetsFormat::SetColorTarget(uint8_t index, dg::TEXTURE_FORMAT format) {
    if (index >= MAX_COLOR_TARGETS) {
        throw EngineError("wrong index {} for TargetsFormat::SetColorTarget, max index is {}", index, MAX_COLOR_TARGETS-1);
    }
    formats[index + 1] = format;
}

void TargetsFormat::SetDepthTarget(dg::TEXTURE_FORMAT format) {
    formats[0] = format;
}

void TargetsFormat::SetCountColorTargets(uint8_t value) {
    if (value > MAX_COLOR_TARGETS) {
        throw EngineError("wrong value {} for TargetsFormat::SetCountColorTargets, max count is {}", value, MAX_COLOR_TARGETS);
    }
    if (value == 0) {
        throw EngineError("wrong value {} for RenderTarget::Update, min count is 0", value);
    }
    countColorTargets = value;
}

struct MaterialBuilder::Impl : Fixed {
    Impl() = delete;
    Impl(const DevicePtr& device, const ContextPtr& context,
        const SwapChainPtr& swapChain, const EngineFactoryPtr& engineFactory, const std::shared_ptr<VDeclStorage>& vDeclStorage);
    ~Impl();

    // valid id more than 0
    uint16_t CacheTargetsFormat(const TargetsFormat& value);
    void FillTargetsFormat(uint16_t targetsId, dg::GraphicsPipelineDesc& gpDesc);
    // valid id more than 0
    const ShaderVar& GetShaderVar(uint16_t textureVarId) const;
    const dg::SamplerDesc& GetSamplerDesc(uint16_t id) const;
    uint16_t CacheShaderVar(const std::string& name, dg::SHADER_TYPE shaderType, dg::SHADER_RESOURCE_VARIABLE_TYPE type, uint16_t samplerId);
    uint16_t CacheShaderVar(uint16_t textureVarId, const dg::SamplerDesc& desc);
    void FillVars(const ShaderVars& vars, dg::PipelineResourceLayoutDesc& desc);

    PipelineStatePtr Create(uint64_t mask, uint16_t targetsId, uint16_t vDeclIdPerVertex, uint16_t vDeclIdPerInstance,
        const ShaderVars& vars, dg::GraphicsPipelineDesc& gpDesc);

    std::vector<TargetsFormat> m_idToTargetsFormat;
    std::unordered_map<TargetsFormat, uint16_t> m_targetsFormatToId;

    uint16_t m_defaultSamplerId;
    std::vector<dg::SamplerDesc> m_idToSampler;
    std::unordered_map<dg::SamplerDesc, uint16_t> m_samplerToId;
    std::vector<ShaderVar> m_idToShaderVar;
    std::unordered_map<ShaderVar, uint16_t> m_shaderVarToId;
    std::unordered_map<PipelineStateKey, PipelineStatePtr> m_pipelineStateCache;

    dg::ShaderResourceVariableDesc m_varsDescs[ShaderVars::max];
    dg::ImmutableSamplerDesc m_samplers[ShaderVars::max];

    DevicePtr m_device;
    SwapChainPtr m_swapChain;
    std::shared_ptr<VDeclStorage> m_vDeclStorage;
    ShaderBuilder* m_shaderBuilder = nullptr;
    MicroshaderLoader* m_microShaderLoader = nullptr;
    StaticVarsStorage* m_staticVarsStorage = nullptr;
};

MaterialBuilder::Impl::Impl(const DevicePtr& device, const ContextPtr& context,
    const SwapChainPtr& swapChain, const EngineFactoryPtr& engineFactory, const std::shared_ptr<VDeclStorage>& vDeclStorage)
    : m_device(device)
    , m_swapChain(swapChain)
    , m_vDeclStorage(vDeclStorage)
    , m_shaderBuilder(new ShaderBuilder(device, engineFactory))
    , m_microShaderLoader(new MicroshaderLoader())
    , m_staticVarsStorage(new StaticVarsStorage(device, context)) {

    dg::SamplerDesc desc;
    desc.AddressU = dg::TEXTURE_ADDRESS_WRAP;
    desc.AddressV = dg::TEXTURE_ADDRESS_WRAP;
    desc.AddressW = dg::TEXTURE_ADDRESS_WRAP;

    m_defaultSamplerId = static_cast<uint16_t>(m_idToShaderVar.size() + 1);
    m_samplerToId[desc] = m_defaultSamplerId;
    m_idToSampler.push_back(desc);
}

MaterialBuilder::Impl::~Impl() {
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

uint16_t MaterialBuilder::Impl::CacheTargetsFormat(const TargetsFormat& value) {
    if (const auto it = m_targetsFormatToId.find(value); it != m_targetsFormatToId.cend()) {
        return it->second;
    }

    auto id = static_cast<uint16_t>(m_idToTargetsFormat.size() + 1);
    m_targetsFormatToId[value] = id;
    m_idToTargetsFormat.push_back(value);

    return id;
}
void MaterialBuilder::Impl::FillTargetsFormat(uint16_t targetsId, dg::GraphicsPipelineDesc& gpDesc) {
    if (targetsId == 0) {
        throw EngineError("MaterialBuilder: wrong targetsId {} for FillTargetsFormat, min targetsId is 1", targetsId);
    }
    if (targetsId > m_idToTargetsFormat.size()) {
        throw EngineError("MaterialBuilder: wrong targetsId {} for FillTargetsFormat, max targetsId is {}", targetsId, m_idToTargetsFormat.size());
    }
    auto& targetsFormat = m_idToTargetsFormat[targetsId - uint16_t(1)];
    gpDesc.DSVFormat = targetsFormat.GetDepthTarget();
    gpDesc.NumRenderTargets = targetsFormat.countColorTargets;
    for (uint8_t i=0; i!=targetsFormat.countColorTargets; ++i) {
        gpDesc.RTVFormats[i] = targetsFormat.formats[i + 1];
    }
}

const ShaderVar& MaterialBuilder::Impl::GetShaderVar(uint16_t textureVarId) const {
    if (textureVarId == 0) {
        throw EngineError("MaterialBuilder: wrong textureVarId {} for GetShaderVar, min textureVarId is 1", textureVarId);
    }

    if (textureVarId > m_idToShaderVar.size()) {
        throw EngineError("MaterialBuilder: wrong textureVarId {} for GetShaderVar, max textureVarId is {}", textureVarId, m_idToShaderVar.size());
    }

    return m_idToShaderVar[textureVarId - uint16_t(1)];
}

const dg::SamplerDesc& MaterialBuilder::Impl::GetSamplerDesc(uint16_t samplerId) const {
    if (samplerId == 0) {
        throw EngineError("MaterialBuilder: wrong samplerId {} for GetSamplerDesc, min samplerId is 1", samplerId);
    }
    if (samplerId > m_idToSampler.size()) {
        throw EngineError("MaterialBuilder: wrong samplerId {} for GetSamplerDesc, max samplerId is {}", samplerId, m_idToSampler.size());
    }

    return m_idToSampler[samplerId - uint16_t(1)];
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
    desc.ImmutableSamplers = m_samplers;
    desc.NumImmutableSamplers = 0;
    for(uint8_t i=0; i!=vars.number; ++i) {
        const auto& shaderVar = GetShaderVar(vars.vars[i]);
        m_varsDescs[i] = {shaderVar.shaderType, shaderVar.name.c_str(), shaderVar.type};
        if (shaderVar.samplerId != 0) {
            const auto& samplerDesc = GetSamplerDesc(shaderVar.samplerId);
            m_samplers[desc.NumImmutableSamplers++] = {shaderVar.shaderType, shaderVar.name.c_str(), samplerDesc};
        }
    }
}

PipelineStatePtr MaterialBuilder::Impl::Create(uint64_t mask, uint16_t targetsId, uint16_t vDeclIdPerVertex, uint16_t vDeclIdPerInstance,
    const ShaderVars& vars, dg::GraphicsPipelineDesc& gpDesc) {

    auto vDeclId = m_vDeclStorage->Join(vDeclIdPerVertex, vDeclIdPerInstance);
    PipelineStateKey key(mask, vDeclId, vars, gpDesc);
    if (const auto it = m_pipelineStateCache.find(key); it != m_pipelineStateCache.cend()) {
        return it->second;
    }

    auto src = m_microShaderLoader->GetSources(mask, m_vDeclStorage->GetSemanticDecls(vDeclId));
    const auto& layoutElements = m_vDeclStorage->GetLayoutElements(vDeclId);
    auto shaders = m_shaderBuilder->Build(src);

    dg::GraphicsPipelineStateCreateInfo createInfo;

    createInfo.PSODesc.Name = "material_builder";
    createInfo.PSODesc.PipelineType = dg::PIPELINE_TYPE_GRAPHICS;
    FillVars(vars, createInfo.PSODesc.ResourceLayout);

    gpDesc.InputLayout = dg::InputLayoutDesc(layoutElements.data(), static_cast<uint32_t>(layoutElements.size()));
    FillTargetsFormat(targetsId, gpDesc);
    createInfo.GraphicsPipeline = gpDesc;

    createInfo.pVS = shaders.vs;
    createInfo.pPS = shaders.ps;
    createInfo.pDS = nullptr;
    createInfo.pHS = nullptr;
    createInfo.pGS = shaders.gs;
    createInfo.pAS = nullptr;
    createInfo.pMS = nullptr;

    PipelineStatePtr pipelineState;
    m_device->CreateGraphicsPipelineState(createInfo, &pipelineState);
    m_staticVarsStorage->SetVars(pipelineState);
    m_pipelineStateCache[key] = pipelineState;

    return pipelineState;
}

MaterialBuilder::MaterialBuilder(const DevicePtr& device, const ContextPtr& context,
    const SwapChainPtr& swapChain, const EngineFactoryPtr& engineFactory, const std::shared_ptr<VDeclStorage>& vDeclStorage)
    : impl(device, context, swapChain, engineFactory, vDeclStorage) {

}

MaterialBuilder::~MaterialBuilder() {

}

uint64_t MaterialBuilder::GetShaderMask(const std::string& name) const {
    return impl->m_microShaderLoader->GetMask(name);
}

void MaterialBuilder::Load(const MaterialBuilderDesc& desc) {
    impl->m_microShaderLoader->Load(desc);
    impl->m_shaderBuilder->Create(desc);
}

uint16_t MaterialBuilder::CacheTargetsFormat(const TargetsFormat& value) {
    return impl->CacheTargetsFormat(value);
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

uint32_t MaterialBuilder::AddGlobalVar(dg::SHADER_TYPE shaderType, const std::string& name, const void* data, size_t dataSize) {
    uint32_t id = impl->m_staticVarsStorage->Add(shaderType, name, dataSize);
    impl->m_staticVarsStorage->Update(id, data, dataSize);
    return id;
}

void MaterialBuilder::UpdateGlobalVar(uint32_t id, const void* data, size_t dataSize) {
    impl->m_staticVarsStorage->Update(id, data, dataSize);
}

PipelineStatePtr MaterialBuilder::Create(uint64_t mask, uint16_t targetsId, uint16_t vDeclIdPerVertex, uint16_t vDeclIdPerInstance,
    const ShaderVars& vars, dg::GraphicsPipelineDesc& gpDesc) {

    return impl->Create(mask, targetsId, vDeclIdPerVertex, vDeclIdPerInstance, vars, gpDesc);
}
