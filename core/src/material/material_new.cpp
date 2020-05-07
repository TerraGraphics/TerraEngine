#include "core/material/material_new.h"

#include <DiligentCore/Graphics/GraphicsEngine/interface/DepthStencilState.h>
#include <DiligentCore/Graphics/GraphicsEngine/interface/ShaderResourceVariable.h>

#include "core/dg/sampler.h"
#include "core/common/exception.h"
#include "core/dg/pipeline_state.h"
#include "core/dg/graphics_types.h"
#include "core/dg/rasterizer_state.h"
#include "core/dg/graphics_accessories.h"
#include "core/material/material_builder.h"

namespace
{

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

struct MaterialNew::Impl {
    Impl(const std::shared_ptr<MaterialBuilder>& builder);

    dg::SamplerDesc& GetTextureDesc(uint8_t id);
    uint8_t AddVar(dg::SHADER_TYPE shaderType, const std::string& name, dg::SHADER_RESOURCE_VARIABLE_TYPE type);
    uint8_t AddTextureVar(dg::SHADER_TYPE shaderType, const std::string& name, dg::SHADER_RESOURCE_VARIABLE_TYPE type);
    void Build(const char* name);

    uint64_t m_mask = 0;
    uint32_t m_vDeclIdPerVertex = 0;
    uint32_t m_vDeclIdPerInstance = 0;
    uint8_t m_varsCount = 0;
    uint8_t m_textureVarsCount = 0;
    MaterialVar m_vars[MaxCountVars];
    TextureVar m_textureVars[MaxCountTextureVars];
    dg::PipelineStateDesc m_desc;
    PipelineStatePtr m_pipelineState;
    std::shared_ptr<MaterialBuilder> m_builder;
};

MaterialNew::Impl::Impl(const std::shared_ptr<MaterialBuilder>& builder)
    : m_builder(builder) {

    m_desc.IsComputePipeline = false;
    m_desc.ResourceLayout.DefaultVariableType = dg::SHADER_RESOURCE_VARIABLE_TYPE_STATIC;

    auto& gp = m_desc.GraphicsPipeline;
    gp.DepthStencilDesc.DepthEnable = true;
    gp.RasterizerDesc.CullMode = dg::CULL_MODE_BACK;
    gp.RasterizerDesc.FrontCounterClockwise = false;
    gp.PrimitiveTopology = dg::PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
}

dg::SamplerDesc& MaterialNew::Impl::GetTextureDesc(uint8_t id) {
    if (m_varsCount >= id) {
        throw EngineError("Material: count of texture shader varaibles is {}, can't get id {}", m_varsCount, id);
    }

    return m_textureVars[id].desc;
}

uint8_t MaterialNew::Impl::AddVar(dg::SHADER_TYPE shaderType, const std::string& name, dg::SHADER_RESOURCE_VARIABLE_TYPE type) {
    if (m_varsCount >= MaxCountVars) {
        throw EngineError("Material: max count of shader varaibles is {}", MaxCountVars);
    }

    for(uint8_t i=0; i!=m_varsCount; ++i) {
        if ((m_vars[i].type == type) && (m_vars[i].name == name)) {
            throw EngineError("Material: shader varaible {} is duplicated for shader type {}", dg::GetShaderTypeLiteralName(shaderType));
        }
    }

    m_vars[m_varsCount] = MaterialVar{name, shaderType, type};

    return m_varsCount++;
}

uint8_t MaterialNew::Impl::AddTextureVar(dg::SHADER_TYPE shaderType, const std::string& name, dg::SHADER_RESOURCE_VARIABLE_TYPE type) {
    if (m_textureVarsCount >= MaxCountTextureVars) {
        throw EngineError("Material: max count of texture shader varaibles is {}", MaxCountTextureVars);
    }

    m_textureVars[m_textureVarsCount] = TextureVar{ AddVar(shaderType, name, type), dg::SamplerDesc{}};

    return m_textureVarsCount++;
}

void MaterialNew::Impl::Build(const char* name) {
    if ((m_mask == 0) && (m_vDeclIdPerVertex == 0) && (m_vDeclIdPerInstance == 0)) {
        throw EngineError("Material: shaders params not be set");
    }

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

    m_desc.Name = (name != nullptr) ? name : "no name is assigned";
    m_pipelineState = m_builder->Create(m_mask, m_vDeclIdPerVertex, m_vDeclIdPerInstance, m_desc);
}

MaterialNew::MaterialNew(const std::shared_ptr<MaterialBuilder>& builder)
    : impl(builder) {

}

MaterialNew::~MaterialNew() {

}

dg::SamplerDesc& MaterialNew::GetTextureDesc(uint8_t id) {
    return impl->GetTextureDesc(id);
}

void MaterialNew::SetShaders(uint64_t mask, uint32_t vDeclIdPerVertex, uint32_t vDeclIdPerInstance) {
    impl->m_mask = mask;
    impl->m_vDeclIdPerVertex = vDeclIdPerVertex;
    impl->m_vDeclIdPerInstance = vDeclIdPerInstance;
    impl->m_textureVarsCount = 0;
    impl->m_varsCount = 0;
}

void MaterialNew::DepthEnable(bool value) noexcept {
    impl->m_desc.GraphicsPipeline.DepthStencilDesc.DepthEnable = value;
}

void MaterialNew::CullMode(dg::CULL_MODE value) noexcept {
    impl->m_desc.GraphicsPipeline.RasterizerDesc.CullMode = value;
}

void MaterialNew::Topology(dg::PRIMITIVE_TOPOLOGY value) noexcept {
    impl->m_desc.GraphicsPipeline.PrimitiveTopology = value;
}

uint8_t MaterialNew::AddVar(dg::SHADER_TYPE shaderType, const std::string& name, dg::SHADER_RESOURCE_VARIABLE_TYPE type) {
    return impl->AddVar(shaderType, name, type);
}

uint8_t MaterialNew::AddTextureVar(dg::SHADER_TYPE shaderType, const std::string& name, dg::SHADER_RESOURCE_VARIABLE_TYPE type) {
    return impl->AddTextureVar(shaderType, name, type);
}

void MaterialNew::Build(const char* name) {
    impl->Build(name);
}
