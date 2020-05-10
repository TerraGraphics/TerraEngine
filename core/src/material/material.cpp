#include "core/material/material.h"

#include <vector>

#include <DiligentCore/Graphics/GraphicsEngine/interface/DepthStencilState.h>
#include <DiligentCore/Graphics/GraphicsEngine/interface/ShaderResourceVariable.h>

#include "core/dg/sampler.h"
#include "core/dg/context.h"
#include "core/common/exception.h"
#include "core/dg/graphics_types.h"
#include "core/dg/rasterizer_state.h"
#include "core/dg/graphics_accessories.h"
#include "core/material/material_builder.h"


namespace {

struct MaterialViewItem {
    MaterialViewItem() = delete;
    MaterialViewItem(uint32_t vDeclSum, MaterialView view) : vDeclSum(vDeclSum), view(view) {};

    uint32_t vDeclSum;
    MaterialView view;
};

struct MaterialVar {
    std::string name;
    dg::SHADER_TYPE shaderType;
    dg::SHADER_RESOURCE_VARIABLE_TYPE type;
};

struct TextureVar {
    uint8_t varId = 0;
    dg::SamplerDesc desc = dg::SamplerDesc{};
};

static constexpr const uint8_t MaxCountVars = 32;
static constexpr const uint8_t MaxCountTextureVars = 16;

}

struct Material::Impl {
    Impl(const std::string& name, const std::shared_ptr<MaterialBuilder>& builder);

    dg::SamplerDesc& GetTextureDesc(uint8_t id);
    uint8_t AddVar(dg::SHADER_TYPE shaderType, const std::string& name, dg::SHADER_RESOURCE_VARIABLE_TYPE type);
    uint8_t AddTextureVar(dg::SHADER_TYPE shaderType, const std::string& name, dg::SHADER_RESOURCE_VARIABLE_TYPE type);

    void SetVertexShaderVar(const char* name, DeviceRaw value);
    void SetPixelShaderVar(const char* name, DeviceRaw value);
    void SetGeometryShaderVar(const char* name, DeviceRaw value);

    MaterialView GetView(uint16_t vDeclIdPerVertex, uint16_t vDeclIdPerInstance, bool& isFind);

    uint64_t m_mask = 0;
    uint8_t m_lastFrameNum = 255;
    uint8_t m_varsCount = 0;
    uint8_t m_textureVarsCount = 0;
    MaterialVar m_vars[MaxCountVars];
    TextureVar m_textureVars[MaxCountTextureVars];
    dg::PipelineStateDesc m_desc;
    std::vector<MaterialViewItem> m_materialViewCache;
    std::string m_name;
    std::shared_ptr<MaterialBuilder> m_builder;
};

Material::Impl::Impl(const std::string& name, const std::shared_ptr<MaterialBuilder>& builder)
    : m_name(name)
    , m_builder(builder) {

    m_desc.Name = m_name.c_str();
    m_desc.IsComputePipeline = false;
    m_desc.ResourceLayout.DefaultVariableType = dg::SHADER_RESOURCE_VARIABLE_TYPE_STATIC;

    auto& gp = m_desc.GraphicsPipeline;
    gp.DepthStencilDesc.DepthEnable = true;
    gp.RasterizerDesc.CullMode = dg::CULL_MODE_BACK;
    gp.RasterizerDesc.FrontCounterClockwise = false;
    gp.PrimitiveTopology = dg::PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
}

dg::SamplerDesc& Material::Impl::GetTextureDesc(uint8_t id) {
    if (m_varsCount <= id) {
        throw EngineError("Material:{}: count of texture shader varaibles is {}, can't get id {}", m_name, m_varsCount, id);
    }

    return m_textureVars[id].desc;
}

uint8_t Material::Impl::AddVar(dg::SHADER_TYPE shaderType, const std::string& name, dg::SHADER_RESOURCE_VARIABLE_TYPE type) {
    if (m_varsCount >= MaxCountVars) {
        throw EngineError("Material: max count of shader varaibles is {}", MaxCountVars);
    }

    for(uint8_t i=0; i!=m_varsCount; ++i) {
        if ((m_vars[i].type == type) && (m_vars[i].name == name)) {
            throw EngineError("Material:{}: shader varaible {} is duplicated for shader type {}", m_name, dg::GetShaderTypeLiteralName(shaderType));
        }
    }

    m_materialViewCache.clear();
    m_vars[m_varsCount] = MaterialVar{name, shaderType, type};

    return m_varsCount++;
}

uint8_t Material::Impl::AddTextureVar(dg::SHADER_TYPE shaderType, const std::string& name, dg::SHADER_RESOURCE_VARIABLE_TYPE type) {
    if (m_textureVarsCount >= MaxCountTextureVars) {
        throw EngineError("Material:{}: max count of texture shader varaibles is {}", m_name, MaxCountTextureVars);
    }

    m_materialViewCache.clear();
    m_textureVars[m_textureVarsCount] = TextureVar{ AddVar(shaderType, name, type), dg::SamplerDesc{}};

    return m_textureVarsCount++;
}

void Material::Impl::SetVertexShaderVar(const char* name, DeviceRaw value) {
    for (auto& item : m_materialViewCache) {
        item.view.SetVertexShaderVar(name, value);
    }
}

void Material::Impl::SetPixelShaderVar(const char* name, DeviceRaw value) {
    for (auto& item : m_materialViewCache) {
        item.view.SetPixelShaderVar(name, value);
    }
}

void Material::Impl::SetGeometryShaderVar(const char* name, DeviceRaw value) {
    for (auto& item : m_materialViewCache) {
        item.view.SetGeometryShaderVar(name, value);
    }
}

MaterialView Material::Impl::GetView(uint16_t vDeclIdPerVertex, uint16_t vDeclIdPerInstance, bool& isFind) {
    uint32_t vDeclSum = (static_cast<uint32_t>(vDeclIdPerVertex) << uint32_t(16)) | static_cast<uint32_t>(vDeclIdPerInstance);

    isFind = true;
    for (auto& item : m_materialViewCache) {
        if (item.vDeclSum == vDeclSum) {
            return item.view;
        }
    }
    isFind = false;

    dg::ShaderResourceVariableDesc vars[MaxCountVars];
    m_desc.ResourceLayout.Variables = vars;
    m_desc.ResourceLayout.NumVariables = m_varsCount;
    for(uint8_t i=0; i!=m_varsCount; ++i) {
        vars[i] = {m_vars[i].shaderType, m_vars[i].name.c_str(), m_vars[i].type};
    }

    dg::StaticSamplerDesc samplers[MaxCountTextureVars];
    m_desc.ResourceLayout.StaticSamplers = samplers;
    m_desc.ResourceLayout.NumStaticSamplers = m_textureVarsCount;
    for(uint8_t i=0; i!=m_textureVarsCount; ++i) {
        samplers[i] = {m_vars[m_textureVars[i].varId].shaderType, m_vars[m_textureVars[i].varId].name.c_str(), m_textureVars[i].desc};
    }

    auto pipelineState = m_builder->Create(m_mask, vDeclIdPerVertex, vDeclIdPerInstance, m_desc);
    ShaderResourceBindingPtr binding;
    pipelineState->CreateShaderResourceBinding(&binding, true);

    auto view = MaterialView(m_name.c_str(), pipelineState, binding);
    m_materialViewCache.emplace_back(vDeclSum, view);

    return view;
}

Material::Material(const std::string& name, const std::shared_ptr<MaterialBuilder>& builder)
    : impl(name, builder) {

}

Material::~Material() {

}

MaterialView Material::GetView(uint8_t frameNum, uint16_t vDeclIdPerVertex, uint16_t vDeclIdPerInstance) {
    if (impl->m_lastFrameNum != frameNum) {
        impl->m_lastFrameNum = frameNum;
        OnNewFrame();
    }
    bool isFind;
    auto view = impl->GetView(vDeclIdPerVertex, vDeclIdPerInstance, isFind);
    if (!isFind) {
        OnNewView(view);
    }

    return view;
}

dg::SamplerDesc& Material::GetTextureDesc(uint8_t id) {
    return impl->GetTextureDesc(id);
}

const std::string& Material::GetName() const noexcept {
    return impl->m_name;
}

uint64_t Material::GetShadersMask() const noexcept {
    return impl->m_mask;
}

void Material::SetShadersMask(uint64_t mask) {
    impl->m_materialViewCache.clear();
    impl->m_mask = mask;
    impl->m_textureVarsCount = 0;
    impl->m_varsCount = 0;
}

void Material::DepthEnable(bool value) noexcept {
    if (impl->m_desc.GraphicsPipeline.DepthStencilDesc.DepthEnable != value) {
        impl->m_materialViewCache.clear();
        impl->m_desc.GraphicsPipeline.DepthStencilDesc.DepthEnable = value;
    }
}

void Material::CullMode(dg::CULL_MODE value) noexcept {
    if (impl->m_desc.GraphicsPipeline.RasterizerDesc.CullMode != value) {
        impl->m_materialViewCache.clear();
        impl->m_desc.GraphicsPipeline.RasterizerDesc.CullMode = value;
    }
}

void Material::Topology(dg::PRIMITIVE_TOPOLOGY value) noexcept {
    if (impl->m_desc.GraphicsPipeline.PrimitiveTopology != value) {
        impl->m_materialViewCache.clear();
        impl->m_desc.GraphicsPipeline.PrimitiveTopology = value;
    }
}

uint8_t Material::AddVar(dg::SHADER_TYPE shaderType, const std::string& name, dg::SHADER_RESOURCE_VARIABLE_TYPE type) {
    return impl->AddVar(shaderType, name, type);
}

uint8_t Material::AddTextureVar(dg::SHADER_TYPE shaderType, const std::string& name, dg::SHADER_RESOURCE_VARIABLE_TYPE type) {
    return impl->AddTextureVar(shaderType, name, type);
}

void Material::SetVertexShaderVar(const char* name, DeviceRaw value) {
    impl->SetVertexShaderVar(name, value);
}

void Material::SetPixelShaderVar(const char* name, DeviceRaw value) {
    impl->SetPixelShaderVar(name, value);
}

void Material::SetGeometryShaderVar(const char* name, DeviceRaw value) {
    impl->SetGeometryShaderVar(name, value);
}
