#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>
#include <cstdint>

#include <DiligentCore/Graphics/GraphicsEngine/interface/ShaderResourceVariable.h>

#include "core/dg/dg.h"
#include "core/dg/sampler.h"
#include "core/common/ctor.h"
#include "core/dg/graphics_types.h"
#include "core/dg/pipeline_state.h"
#include "core/material/vertex_decl.h"
#include "core/material/material_vars.h"


class Material;
class VDeclStorage;
class ShaderBuilder;
class MicroshaderLoader;
struct MaterialBuilderDesc;
class MaterialBuilder : Fixed {
public:
    struct Builder : Fixed {
        struct ShaderResourceVariableDescKey {
            bool operator<(const ShaderResourceVariableDescKey& other) const noexcept;

            dg::SHADER_TYPE shaderType;
            std::string name;
        };

        struct StaticSamplerDesc {
            StaticSamplerDesc(dg::SHADER_TYPE shaderType, const std::string& name, const dg::SamplerDesc& desc);

            dg::SHADER_TYPE shaderType;
            std::string name;
            dg::SamplerDesc desc;
        };

    public:
        Builder() = delete;
        Builder(MaterialBuilder* builder, dg::PipelineStateDesc&& desc);
        ~Builder() = default;

        Builder& DepthEnable(bool value) noexcept;
        Builder& CullMode(dg::CULL_MODE value) noexcept;
        Builder& Topology(dg::PRIMITIVE_TOPOLOGY value) noexcept;
        Builder& Var(dg::SHADER_TYPE shaderType, const std::string& name, dg::SHADER_RESOURCE_VARIABLE_TYPE type = dg::SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE) noexcept;
        Builder& TextureVar(dg::SHADER_TYPE shaderType, const std::string& name, const dg::SamplerDesc& desc, dg::SHADER_RESOURCE_VARIABLE_TYPE type = dg::SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE) noexcept;
        Builder& TextureVar(dg::SHADER_TYPE shaderType, const std::string& name, dg::TEXTURE_ADDRESS_MODE addressMode, dg::SHADER_RESOURCE_VARIABLE_TYPE type = dg::SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE) noexcept;

        std::shared_ptr<Material> Build(const char* name = nullptr);

    private:
        MaterialBuilder* m_builder;
        std::map<ShaderResourceVariableDescKey, dg::SHADER_RESOURCE_VARIABLE_TYPE> m_vars;
        std::vector<StaticSamplerDesc> m_samplers;
        dg::PipelineStateDesc m_desc;
    };

public:
    MaterialBuilder() = delete;
    MaterialBuilder(const DevicePtr& device, const ContextPtr& context,
        const SwapChainPtr& swapChain, const EngineFactoryPtr& engineFactory,
        const std::shared_ptr<VDeclStorage>& vDeclStorage);
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

    Builder Create(uint64_t mask, uint32_t vDeclVertex, uint32_t vDeclinstance);

private:
    std::shared_ptr<Material> Build(dg::PipelineStateDesc& desc);

    DevicePtr m_device;
    SwapChainPtr m_swapChain;
    std::shared_ptr<VDeclStorage> m_vDeclStorage;
    VertexDeclCache* m_vertexDeclCache = nullptr;
    ShaderBuilder* m_shaderBuilder = nullptr;
    MicroshaderLoader* m_microShaderLoader = nullptr;
    StaticVarsStorage* m_staticVarsStorage = nullptr;
};
