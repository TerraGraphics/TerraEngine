#include "middleware/std_render/std_material.h"

#include <memory>

#include <DiligentCore/Graphics/GraphicsEngine/interface/TextureView.h>

#include "core/engine.h"
#include "core/dg/buffer.h"
#include "core/dg/shader.h"
#include "core/dg/device.h"
#include "core/dg/context.h"
#include "core/dg/sampler.h"
#include "core/dg/graphics_types.h"
#include "core/material/material_view.h"
#include "core/material/material_builder.h"
#include "core/dg/shader_resource_variable.h"


namespace {

static uint64_t BaseColorMaterialMask() {
    static auto value = Engine::Get().GetMaterialBuilder()->GetShaderMask("BASE_COLOR_MATERIAL");
    return value;
}

static uint64_t BaseColorTextureMask() {
    static auto value = Engine::Get().GetMaterialBuilder()->GetShaderMask("BASE_COLOR_TEXTURE");
    return value;
}

static uint64_t AlphaTestMask() {
    static auto value = Engine::Get().GetMaterialBuilder()->GetShaderMask("ALPHA_TEST");
    return value;
}

static uint64_t AmbientDiffusePhongMask() {
    static auto value = Engine::Get().GetMaterialBuilder()->GetShaderMask("AMBIENT_DIFFUSE_PHONG");
    return value;
}

static uint64_t SpecularPhongMask() {
    static auto value = Engine::Get().GetMaterialBuilder()->GetShaderMask("SPECULAR_PHONG");
    return value;
}

static uint64_t ColorPickerMask() {
    static auto value = Engine::Get().GetMaterialBuilder()->GetShaderMask("COLOR_PICKER");
    return value;
}

static uint64_t GrassMask() {
    static auto value = Engine::Get().GetMaterialBuilder()->GetShaderMask("GRASS");
    return value;
}

}

StdMaterial::StdMaterial(const std::string& name)
    : Material(name, Engine::Get().GetMaterialBuilder()) {

    m_data.alphaThreshold = -1.f;
    m_materialVarId = GetBuilder()->CacheShaderVar("Material", dg::SHADER_TYPE_PIXEL, dg::SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE);
    m_baseTextureVarId = GetBuilder()->CacheTextureVar("texBase", dg::SHADER_TYPE_PIXEL, dg::SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE);
    ApplyMask(BaseColorMaterialMask());
}

StdMaterial::~StdMaterial() {

}

void StdMaterial::Depth(bool enable) noexcept {
    DepthEnable(enable);
}

void StdMaterial::SetCullMode(dg::CULL_MODE value) noexcept {
    CullMode(value);
}

void StdMaterial::SetTopology(dg::PRIMITIVE_TOPOLOGY value) noexcept {
    Topology(value);
}

void StdMaterial::SetBaseColor(const math::Color4f& value) {
    m_data.crlBase = value.ToFloat4();
}

void StdMaterial::SetBaseColor(const dg::float4& value) {
    m_data.crlBase = value;
}

void StdMaterial::SetBaseColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    m_data.crlBase = math::Color4f(r, g, b, a).ToFloat4();
}

const dg::SamplerDesc& StdMaterial::GetBaseTextureDesc() {
    return GetBuilder()->GetCachedSamplerDesc(m_baseTextureVarId);
}

void StdMaterial::SetBaseTextureDesc(const dg::SamplerDesc& desc) {
    m_baseTextureVarId = GetBuilder()->CacheTextureVar(m_baseTextureVarId, desc);
}

void StdMaterial::SetBaseTextureAddressMode(dg::TEXTURE_ADDRESS_MODE mode) {
    dg::SamplerDesc desc = GetBaseTextureDesc();
    desc.AddressU = mode;
    desc.AddressV = mode;
    desc.AddressW = mode;
    SetBaseTextureDesc(desc);
}

void StdMaterial::SetBaseTexture(TextureViewPtr& texture) {
    if (m_baseTexture == texture) {
        return;
    }

    m_baseTexture = texture;
    if (!texture) {
        AddAndRemoveFlag(BaseColorMaterialMask(), BaseColorTextureMask());
    } else if (!AddAndRemoveFlag(BaseColorTextureMask(), BaseColorMaterialMask())) {
        SetPixelShaderVar("texBase", m_baseTexture);
    }
}

void StdMaterial::SetAlphaThreshold(float value) {
    m_data.alphaThreshold = value;
    if (value > 0.0) {
        AddFlag(AlphaTestMask());
    } else {
        RemoveFlag(AlphaTestMask());
    }
}

void StdMaterial::AmbientDiffuse(bool enable) {
    if (enable) {
        AddFlag(AmbientDiffusePhongMask());
    } else {
        RemoveFlag(AmbientDiffusePhongMask());
    }
}

void StdMaterial::Specular(bool enable) {
    if (enable) {
        AddFlag(SpecularPhongMask());
    } else {
        RemoveFlag(SpecularPhongMask());
    }
}

void StdMaterial::ColorPicker(bool enable) {
    if (enable) {
        AddFlag(ColorPickerMask());
    } else {
        RemoveFlag(ColorPickerMask());
    }
}

void StdMaterial::OnNewFrame() {
    if (!m_dataEnable) {
        return;
    }

    auto& context = Engine::Get().GetContext();
    void* data = nullptr;
    context->MapBuffer(m_buffer, dg::MAP_WRITE, dg::MAP_FLAG_DISCARD, data);
    *reinterpret_cast<dg::ShaderMaterial*>(data) = m_data;
    context->UnmapBuffer(m_buffer, dg::MAP_WRITE);
}

void StdMaterial::OnNewView(MaterialView& view) {
    uint64_t mask = GetShadersMask();

    if (m_dataEnable) {
        view.SetPixelShaderVar("Material", m_buffer);
    }

    if ((mask & BaseColorTextureMask()) != 0) {
        view.SetPixelShaderVar("texBase", m_baseTexture);
    }
}

void StdMaterial::ApplyMask(uint64_t mask) {
    SetShadersMask(mask);

    auto materialEnableFlags = (AlphaTestMask() | BaseColorMaterialMask());
    m_dataEnable = ((mask & materialEnableFlags) != 0);

    if (m_dataEnable && (!m_buffer)) {
        dg::BufferDesc desc;
        desc.Name = "cb::ps::shader_material";
        desc.uiSizeInBytes = sizeof(dg::ShaderMaterial);
        desc.Usage = dg::USAGE_DYNAMIC;
        desc.BindFlags = dg::BIND_UNIFORM_BUFFER;
        desc.CPUAccessFlags = dg::CPU_ACCESS_WRITE;
        Engine::Get().GetDevice()->CreateBuffer(desc, nullptr, &m_buffer);
    }

    if (m_dataEnable) {
        AddShaderVar(m_materialVarId);
    }

    if ((mask & BaseColorTextureMask()) != 0) {
        AddShaderVar(m_baseTextureVarId);
    }
}

void StdMaterial::AddFlag(uint64_t flag) {
    uint64_t mask = GetShadersMask();
    if ((mask & flag) == 0) {
        mask |= flag;
        ApplyMask(mask);
    }
}

void StdMaterial::RemoveFlag(uint64_t flag) {
    uint64_t mask = GetShadersMask();
    if ((mask & flag) != 0) {
        mask &= ~flag;
        ApplyMask(mask);
    }
}

bool StdMaterial::AddAndRemoveFlag(uint64_t addFlag, uint64_t removeFlag) {
    uint64_t mask = GetShadersMask();
    uint64_t newMask = ((mask | addFlag) & ~removeFlag);
    if (mask != newMask) {
        ApplyMask(newMask);
        return true;
    }

    return false;
}

StdMaterialGrass::StdMaterialGrass(const std::string& name)
    : StdMaterial(name) {
    AddFlag(GrassMask());
}

StdMaterialGrass::~StdMaterialGrass() {

}
