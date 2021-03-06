#pragma once

#include <memory>
#include <string>
#include <cstdint>
#include <cstddef>

#include "dg/dg.h"
#include "core/common/ctor.h"
#include "core/common/pimpl.h"


struct ShaderVars {
    bool operator==(const ShaderVars& other) const;

    static constexpr const uint8_t max = 32;

    uint16_t vars[max];
    uint8_t number = 0;
};

struct TargetsFormat {
    bool operator==(const TargetsFormat& other) const;

    dg::TEXTURE_FORMAT GetDepthTarget() const noexcept;

    void SetColorTarget(uint8_t index, dg::TEXTURE_FORMAT format);
    void SetDepthTarget(dg::TEXTURE_FORMAT format);
    void SetCountColorTargets(uint8_t value);

    static constexpr const uint8_t MAX_COLOR_TARGETS = 2;

    dg::TEXTURE_FORMAT formats[MAX_COLOR_TARGETS + 1];
    uint8_t countColorTargets = 0;
};

namespace Diligent {
    struct SamplerDesc;
    struct GraphicsPipelineDesc;
}
class VDeclStorage;
struct MaterialBuilderDesc;
class MaterialBuilder : Fixed {
public:
    MaterialBuilder() = delete;
    MaterialBuilder(const DevicePtr& device, const ContextPtr& context,
        const SwapChainPtr& swapChain, const EngineFactoryPtr& engineFactory,
        const std::shared_ptr<VDeclStorage>& vDeclStorage);
    ~MaterialBuilder();

    uint64_t GetShaderMask(const std::string& name) const;

    void Load(const MaterialBuilderDesc& desc);

    // valid id more than 0
    uint16_t CacheTargetsFormat(const TargetsFormat& value);

    uint16_t CacheShaderVar(const std::string& name, dg::SHADER_TYPE shaderType, dg::SHADER_RESOURCE_VARIABLE_TYPE type);
    uint16_t CacheTextureVar(const std::string& name, dg::SHADER_TYPE shaderType, dg::SHADER_RESOURCE_VARIABLE_TYPE type);
    uint16_t CacheTextureVar(uint16_t textureVarId, const dg::SamplerDesc& desc);
    const dg::SamplerDesc& GetCachedSamplerDesc(uint16_t textureVarId) const;

    uint32_t AddGlobalVar(dg::SHADER_TYPE shaderType, const std::string& name, const void* data, size_t dataSize);
    template<typename T> uint32_t AddGlobalVar(dg::SHADER_TYPE shaderType, const std::string& name) {
        T data;
        return AddGlobalVar(shaderType, name, reinterpret_cast<const void*>(&data), sizeof(T));
    }

    void UpdateGlobalVar(uint32_t id, const void* data, size_t dataSize);
    template<typename T> void UpdateGlobalVar(uint32_t id, const T& data) {
        UpdateGlobalVar(id, reinterpret_cast<const void*>(&data), sizeof(T));
    }

    PipelineStatePtr Create(uint64_t mask, uint16_t targetsId, uint16_t vDeclIdPerVertex, uint16_t vDeclIdPerInstance,
        const ShaderVars& vars, dg::GraphicsPipelineDesc& gpDesc);

private:
    struct Impl;
    Pimpl<Impl, 3432, 8> impl;
};
