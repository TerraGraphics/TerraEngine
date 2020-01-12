#pragma once

#include <map>
#include <DiligentCore/Graphics/GraphicsEngine/interface/PipelineState.h>

#include "core/material/material.h"
#include "core/material/material_builder_desc.h"


class ShaderBuilder;
class MicroShaderLoader;
class StaticVarsStorage;
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
        Builder(MaterialBuilder* builder, dg::RefCntAutoPtr<dg::IShader>& shaderVS, dg::RefCntAutoPtr<dg::IShader>& shaderPS, const dg::InputLayoutDesc& layoutDesc);
        ~Builder() = default;

        Builder& CullMode(dg::CULL_MODE value) noexcept;
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
    ~MaterialBuilder() = default;

    std::shared_ptr<StaticVarsStorage> GetStaticVarsStorage() { return m_staticVarsStorage; }
    uint64_t GetShaderMask(const std::string& name) const;

    void Load(const MaterialBuilderDesc& desc);
    Builder Create(uint64_t mask, const dg::InputLayoutDesc& layoutDesc);

private:
    std::shared_ptr<Material> Build(dg::PipelineStateDesc& desc);

    DevicePtr m_device;
    SwapChainPtr m_swapChain;
    std::unique_ptr<ShaderBuilder> m_shaderBuilder;
    std::unique_ptr<MicroShaderLoader> m_microShaderLoader;
    std::shared_ptr<StaticVarsStorage> m_staticVarsStorage = nullptr;
};
