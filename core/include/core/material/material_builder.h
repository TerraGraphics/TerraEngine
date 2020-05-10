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
#include "core/material/material_vars.h"


class Material;
class VDeclStorage;
class ShaderBuilder;
class MicroshaderLoader;
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

    template<typename T> uint32_t AddGlobalVar(dg::SHADER_TYPE shaderType, const std::string& name) {
        T data;
        uint32_t id = m_staticVarsStorage->Add(shaderType, name, sizeof(T));
        m_staticVarsStorage->Update(id, reinterpret_cast<const void*>(&data), sizeof(T));
        return id;
    }

    template<typename T> void UpdateGlobalVar(uint32_t id, const T& data) {
        m_staticVarsStorage->Update(id, reinterpret_cast<const void*>(&data), sizeof(T));
    }

    PipelineStatePtr Create(uint64_t mask, uint16_t vDeclIdPerVertex, uint16_t vDeclIdPerInstance, dg::PipelineStateDesc& desc);

private:
    DevicePtr m_device;
    SwapChainPtr m_swapChain;
    std::shared_ptr<VDeclStorage> m_vDeclStorage;
    ShaderBuilder* m_shaderBuilder = nullptr;
    MicroshaderLoader* m_microShaderLoader = nullptr;
    StaticVarsStorage* m_staticVarsStorage = nullptr;
};
