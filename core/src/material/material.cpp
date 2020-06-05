#include "core/material/material.h"

#include <vector>

#include "dg/context.h"
#include "dg/graphics_types.h"
#include "dg/rasterizer_state.h"
#include "core/common/exception.h"
#include "core/material/material_builder.h"


namespace {

struct MaterialViewItem {
    MaterialViewItem() = delete;
    MaterialViewItem(uint32_t cacheId, MaterialView view) : cacheId(cacheId), view(view) {};

    uint64_t cacheId;
    MaterialView view;
};

}

struct Material::Impl {
    Impl(Material* const material, const std::string& name, const std::shared_ptr<MaterialBuilder>& builder);

    void AddShaderVar(uint16_t varId);
    void SetVertexShaderVar(const char* name, DeviceRaw value);
    void SetPixelShaderVar(const char* name, DeviceRaw value);
    void SetGeometryShaderVar(const char* name, DeviceRaw value);

    MaterialView GetView(uint16_t targetsId, uint16_t vDeclIdPerVertex, uint16_t vDeclIdPerInstance);

    Material* const m_this = nullptr;
    uint64_t m_mask = 0;
    uint8_t m_lastFrameNum = 255;
    ShaderVars m_vars;
    dg::GraphicsPipelineDesc m_desc;
    std::vector<MaterialViewItem> m_materialViewCache;
    std::string m_name;
    std::shared_ptr<MaterialBuilder> m_builder;
};

Material::Impl::Impl(Material* const material, const std::string& name, const std::shared_ptr<MaterialBuilder>& builder)
    : m_this(material)
    , m_name(name)
    , m_builder(builder) {

    m_vars.number = 0;

    m_desc.DepthStencilDesc.DepthEnable = true;
    m_desc.RasterizerDesc.CullMode = dg::CULL_MODE_BACK;
    m_desc.RasterizerDesc.FrontCounterClockwise = false;
    m_desc.PrimitiveTopology = dg::PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
}

void Material::Impl::AddShaderVar(uint16_t varId) {
    if (m_vars.number >= ShaderVars::max) {
        throw EngineError("Material:{}: max count of shader varaibles is {}", m_name, ShaderVars::max);
    }
    m_materialViewCache.clear();
    m_vars.vars[m_vars.number++] = varId;
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

MaterialView Material::Impl::GetView(uint16_t targetsId, uint16_t vDeclIdPerVertex, uint16_t vDeclIdPerInstance) {
    uint64_t cacheId = (static_cast<uint64_t>(targetsId) << uint64_t(32)) | (static_cast<uint64_t>(vDeclIdPerVertex) << uint64_t(16)) | static_cast<uint64_t>(vDeclIdPerInstance);

    for (auto& item : m_materialViewCache) {
        if (item.cacheId == cacheId) {
            return item.view;
        }
    }

    auto mask = m_this->OnBeforeCreateView(vDeclIdPerVertex, vDeclIdPerInstance);
    auto pipelineState = m_builder->Create(mask, targetsId, vDeclIdPerVertex, vDeclIdPerInstance, m_vars, m_desc);
    ShaderResourceBindingPtr binding;
    pipelineState->CreateShaderResourceBinding(&binding, true);

    auto view = MaterialView(m_name.c_str(), pipelineState, binding);
    m_materialViewCache.emplace_back(cacheId, view);
    m_this->OnAfterCreateView(view);

    return view;
}

Material::Material(const std::string& name, const std::shared_ptr<MaterialBuilder>& builder)
    : impl(this, name, builder) {

}

Material::~Material() {

}

MaterialView Material::GetView(uint8_t frameNum, uint16_t targetsId, uint16_t vDeclIdPerVertex, uint16_t vDeclIdPerInstance) {
    if (impl->m_lastFrameNum != frameNum) {
        impl->m_lastFrameNum = frameNum;
        OnNewFrame();
    }
    return impl->GetView(targetsId, vDeclIdPerVertex, vDeclIdPerInstance);
}

const std::string& Material::GetName() const noexcept {
    return impl->m_name;
}

std::shared_ptr<MaterialBuilder>& Material::GetBuilder() noexcept {
    return impl->m_builder;
}

void Material::ResetCache() {
    impl->m_materialViewCache.clear();
}

uint64_t Material::GetShadersMask() const noexcept {
    return impl->m_mask;
}

void Material::SetShadersMask(uint64_t mask) {
    impl->m_materialViewCache.clear();
    impl->m_mask = mask;
    impl->m_vars.number = 0;
}

void Material::DepthEnable(bool value) noexcept {
    if (impl->m_desc.DepthStencilDesc.DepthEnable != value) {
        impl->m_materialViewCache.clear();
        impl->m_desc.DepthStencilDesc.DepthEnable = value;
    }
}

void Material::CullMode(dg::CULL_MODE value) noexcept {
    if (impl->m_desc.RasterizerDesc.CullMode != value) {
        impl->m_materialViewCache.clear();
        impl->m_desc.RasterizerDesc.CullMode = value;
    }
}

void Material::Topology(dg::PRIMITIVE_TOPOLOGY value) noexcept {
    if (impl->m_desc.PrimitiveTopology != value) {
        impl->m_materialViewCache.clear();
        impl->m_desc.PrimitiveTopology = value;
    }
}

void Material::AddShaderVar(uint16_t varId) {
    impl->AddShaderVar(varId);
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
