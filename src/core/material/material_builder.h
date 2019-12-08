#pragma once

#include <map>
#include <vector>
#include <memory>

#include <DiligentCore/Graphics/GraphicsEngine/interface/PipelineState.h>

#include "core/material/material.h"


namespace Diligent {
    class ISwapChain;
}

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
    MaterialBuilder(const DevicePtr& device, const dg::RefCntAutoPtr<dg::ISwapChain>& swapChain);
    ~MaterialBuilder() = default;

    Builder Create(dg::RefCntAutoPtr<dg::IShader>& shaderVS, dg::RefCntAutoPtr<dg::IShader>& shaderPS, const dg::InputLayoutDesc& layoutDesc);
private:
    std::shared_ptr<Material> Build(dg::PipelineStateDesc& desc);

    DevicePtr m_device;
    dg::RefCntAutoPtr<dg::ISwapChain> m_swapChain;
};
