#pragma once

#include <map>
#include <DiligentCore/Graphics/GraphicsEngine/interface/PipelineState.h>

#include "core/material/material.h"
#include "core/material/material_vars.h"
#include "core/material/material_builder_desc.h"


class ShaderBuilder;
class MicroshaderLoader;
class MicroshaderLoaderOld;
class MaterialBuilder : Fixed {
public:
    struct Builder : Fixed {
        struct ShaderResourceVariableDescKey {
            bool operator<(const ShaderResourceVariableDescKey& other) const noexcept;

            dg::SHADER_TYPE shaderType;
            dg::String name;
        };

        struct StaticSamplerDesc {
            StaticSamplerDesc(dg::SHADER_TYPE shaderType, const dg::String& name, const dg::SamplerDesc& desc);

            dg::SHADER_TYPE shaderType;
            dg::String name;
            dg::SamplerDesc desc;
        };

    public:
        Builder() = delete;
        Builder(MaterialBuilder* builder, ShaderPtr& shaderVS, ShaderPtr& shaderPS, ShaderPtr& shaderGS, const dg::InputLayoutDesc& layoutDesc);
        ~Builder() = default;

        Builder& DepthEnable(bool value) noexcept;
        Builder& CullMode(dg::CULL_MODE value) noexcept;
        Builder& Topology(dg::PRIMITIVE_TOPOLOGY value) noexcept;
        Builder& Var(dg::SHADER_TYPE shaderType, const dg::String& name, dg::SHADER_RESOURCE_VARIABLE_TYPE type = dg::SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE) noexcept;
        Builder& TextureVar(dg::SHADER_TYPE shaderType, const dg::String& name, const dg::SamplerDesc& desc, dg::SHADER_RESOURCE_VARIABLE_TYPE type = dg::SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE) noexcept;
        Builder& TextureVar(dg::SHADER_TYPE shaderType, const dg::String& name, dg::TEXTURE_ADDRESS_MODE addressMode, dg::SHADER_RESOURCE_VARIABLE_TYPE type = dg::SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE) noexcept;

        std::shared_ptr<Material> Build(const dg::Char* name = nullptr);

    private:
        MaterialBuilder* m_builder;
        std::map<ShaderResourceVariableDescKey, dg::SHADER_RESOURCE_VARIABLE_TYPE> m_vars;
        std::vector<StaticSamplerDesc> m_samplers;
        dg::PipelineStateDesc m_desc;
    };

public:
    MaterialBuilder() = delete;
    MaterialBuilder(const DevicePtr& device, const ContextPtr& context, const SwapChainPtr& swapChain, const EngineFactoryPtr& engineFactory);
    ~MaterialBuilder();

    uint64_t GetShaderMask(const std::string& name) const;

    void Load(const MaterialBuilderDesc& desc);

    template<typename T> uint32_t AddGlobalVar(dg::SHADER_TYPE shaderType, const std::string& name) {
        T data;
        uint32_t id = m_staticVarsStorage->Add(shaderType, name, sizeof(T));
        m_staticVarsStorage->Update(id, reinterpret_cast<const void*>(&data), sizeof(T));
        return id;
    }

    template<typename T> void UpdateGlobalVar(uint32_t id, const T& data) {
        m_staticVarsStorage->Update(id, reinterpret_cast<const void*>(&data), sizeof(T));
    }

    Builder Create(uint64_t mask, const dg::InputLayoutDesc& layoutDesc);

private:
    std::shared_ptr<Material> Build(dg::PipelineStateDesc& desc);

    DevicePtr m_device;
    SwapChainPtr m_swapChain;
    ShaderBuilder* m_shaderBuilder = nullptr;
    MicroshaderLoader* m_microShaderLoader = nullptr;
    MicroshaderLoaderOld* m_microShaderLoaderOld = nullptr;
    StaticVarsStorage* m_staticVarsStorage = nullptr;
};
