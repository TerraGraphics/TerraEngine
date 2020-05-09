#include "middleware/std_material/std_material_new.h"

#include <memory>

#include <DiligentCore/Graphics/GraphicsEngine/interface/Shader.h>
#include <DiligentCore/Graphics/GraphicsEngine/interface/TextureView.h>
#include <DiligentCore/Graphics/GraphicsEngine/interface/ShaderResourceVariable.h>

#include "core/engine.h"
#include "core/dg/buffer.h"
#include "core/dg/device.h"
#include "core/dg/context.h"
#include "core/dg/graphics_types.h"
#include "core/material/material_view.h"
#include "core/material/material_builder.h"


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

}

StdMaterialNew::StdMaterialNew(const std::string& name)
    : MaterialNew(name, Engine::Get().GetMaterialBuilder()) {

    m_data.alphaThreshold = -1.f;

    dg::BufferDesc desc;
    desc.Name = "cb::ps::shader_material";
    desc.uiSizeInBytes = sizeof(dg::ShaderMaterial);
    desc.Usage = dg::USAGE_DYNAMIC;
    desc.BindFlags = dg::BIND_UNIFORM_BUFFER;
    desc.CPUAccessFlags = dg::CPU_ACCESS_WRITE;
    Engine::Get().GetDevice()->CreateBuffer(desc, nullptr, &m_buffer);

    SetShadersMask(BaseColorMaterialMask());
    FillMeta();
}

StdMaterialNew::~StdMaterialNew() {

}

void StdMaterialNew::SetBaseColor(const math::Color4f& value) {
    m_data.crlBase = value.ToFloat4();
}

void StdMaterialNew::SetBaseColor(const dg::float4& value) {
    m_data.crlBase = value;
}

void StdMaterialNew::SetBaseColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    m_data.crlBase = math::Color4f(r, g, b, a).ToFloat4();
}

void StdMaterialNew::SetBaseTexture(TextureViewPtr& texture) {
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

void StdMaterialNew::SetAlphaThreshold(float value) {
    m_data.alphaThreshold = value;
    if (value > 0.0) {
        AddFlag(AlphaTestMask());
    } else {
        RemoveFlag(AlphaTestMask());
    }
}

void StdMaterialNew::AmbientDiffuse(bool enable) {
    if (enable) {
        AddFlag(AmbientDiffusePhongMask());
    } else {
        RemoveFlag(AmbientDiffusePhongMask());
    }
}

void StdMaterialNew::Specular(bool enable) {
    if (enable) {
        AddFlag(SpecularPhongMask());
    } else {
        RemoveFlag(SpecularPhongMask());
    }
}

void StdMaterialNew::OnNewFrame() {
    auto& context = Engine::Get().GetContext();

    void* data = nullptr;
    context->MapBuffer(m_buffer, dg::MAP_WRITE, dg::MAP_FLAG_DISCARD, data);
    *reinterpret_cast<dg::ShaderMaterial*>(data) = m_data;
    context->UnmapBuffer(m_buffer, dg::MAP_WRITE);
}

void StdMaterialNew::OnNewView(MaterialView& view) {
    uint64_t mask = GetShadersMask();

    view.SetPixelShaderVar("Material", m_buffer);
    if ((mask & BaseColorTextureMask()) != 0) {
        SetPixelShaderVar("texBase", m_baseTexture);
    }
}

void StdMaterialNew::FillMeta() {
    uint64_t mask = GetShadersMask();

    AddVar(dg::SHADER_TYPE_PIXEL, "Material", dg::SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE);
    if ((mask & BaseColorTextureMask()) != 0) {
        AddTextureVar(dg::SHADER_TYPE_PIXEL, "texBase", dg::SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE);
    }
}

void StdMaterialNew::AddFlag(uint64_t flag) {
    uint64_t mask = GetShadersMask();
    if ((mask & flag) == 0) {
        mask |= flag;
        SetShadersMask(mask);
        FillMeta();
    }
}

void StdMaterialNew::RemoveFlag(uint64_t flag) {
    uint64_t mask = GetShadersMask();
    if ((mask & flag) != 0) {
        mask &= ~flag;
        SetShadersMask(mask);
        FillMeta();
    }
}

bool StdMaterialNew::AddAndRemoveFlag(uint64_t addFlag, uint64_t removeFlag) {
    uint64_t mask = GetShadersMask();
    uint64_t newMask = ((mask | addFlag) & ~removeFlag);
    if (mask != newMask) {
        SetShadersMask(newMask);
        FillMeta();
        return true;
    }

    return false;
}
