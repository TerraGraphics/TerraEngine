#include "middleware/std_material/std_material.h"

#include <DiligentCore/Graphics/GraphicsEngine/interface/Shader.h>
#include <DiligentCore/Graphics/GraphicsEngine/interface/TextureView.h>
#include <DiligentCore/Graphics/GraphicsEngine/interface/ShaderResourceBinding.h>
#include <DiligentCore/Graphics/GraphicsEngine/interface/ShaderResourceVariable.h>

#include "core/dg/buffer.h"
#include "core/dg/device.h"
#include "core/dg/context.h"
#include "core/dg/graphics_types.h"


StdMaterial::StdMaterial(const std::shared_ptr<Material>& material)
    : MaterialNode(material) {

}

StdMaterial::~StdMaterial() {

}

void StdMaterial::SetBaseColor(const math::Color4f& value) {
    m_materialData.crlBase = value.ToFloat4();
}

void StdMaterial::SetBaseColor(const dg::float4& value) {
    m_materialData.crlBase = value;
}

void StdMaterial::SetBaseColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    m_materialData.crlBase = math::Color4f(r, g, b, a).ToFloat4();
}

void StdMaterial::SetBaseTexture(TextureViewPtr& texture) {
    SetPixelShaderVar("texBase", texture);
}

void StdMaterial::SetAlphaThreshold(float value) {
    m_materialData.alphaThreshold = value;
}

void StdMaterial::Update(DevicePtr& device, ContextPtr& context) {
    if (!m_isMaterialDataExists) {
        return;
    }

    if (!m_materialCB) {
        auto* var = m_binding->GetVariableByName(dg::SHADER_TYPE_PIXEL, "Material");
        if (!var) {
            m_isMaterialDataExists = false;
            return;
        }

        dg::BufferDesc desc;
        desc.Name = "cb::ps::shader_material";
        desc.uiSizeInBytes = sizeof(dg::ShaderMaterial);
        desc.Usage = dg::USAGE_DYNAMIC;
        desc.BindFlags = dg::BIND_UNIFORM_BUFFER;
        desc.CPUAccessFlags = dg::CPU_ACCESS_WRITE;
        device->CreateBuffer(desc, nullptr, &m_materialCB);

        var->Set(m_materialCB);
    }

    void* data = nullptr;
    context->MapBuffer(m_materialCB, dg::MAP_WRITE, dg::MAP_FLAG_DISCARD, data);
    *reinterpret_cast<dg::ShaderMaterial*>(data) = m_materialData;
    context->UnmapBuffer(m_materialCB, dg::MAP_WRITE);
}
