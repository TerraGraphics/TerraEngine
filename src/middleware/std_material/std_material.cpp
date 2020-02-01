#include "middleware/std_material/std_material.h"

#include <DiligentCore/Graphics/GraphicsEngine/interface/RenderDevice.h>
#include <DiligentCore/Graphics/GraphicsEngine/interface/DeviceContext.h>


StdMaterial::StdMaterial(const std::shared_ptr<Material>& material, const std::shared_ptr<GeometryNode>& geometry)
    : MaterialNode(material, geometry) {

}

void StdMaterial::SetBaseColor(const Color4f& value) {
    m_materialData.crlBase = value.ToFloat4();
    m_isDirty = true;
}

void StdMaterial::SetBaseColor(const dg::float4& value) {
    m_materialData.crlBase = value;
    m_isDirty = true;
}

void StdMaterial::SetBaseColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    m_materialData.crlBase = Color4f(r, g, b, a).ToFloat4();
    m_isDirty = true;
}

void StdMaterial::SetBaseTexture(TextureViewPtr& texture) {
    SetPixelShaderVar("texBase", texture);
}

void StdMaterial::SetAlphaThreshold(float value) {
    m_materialData.alphaThreshold = value;
    m_isDirty = true;
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

        dg::ShaderMaterial* data;
        context->MapBuffer(m_materialCB, dg::MAP_WRITE, dg::MAP_FLAG_DISCARD, (dg::PVoid&)data);
        *data = m_materialData;
        context->UnmapBuffer(m_materialCB, dg::MAP_WRITE);

        var->Set(m_materialCB);
        m_isDirty = false;
    }

    if (m_isDirty) {
        dg::ShaderMaterial* data;
        context->MapBuffer(m_materialCB, dg::MAP_WRITE, dg::MAP_FLAG_DISCARD, (dg::PVoid&)data);
        *data = m_materialData;
        context->UnmapBuffer(m_materialCB, dg::MAP_WRITE);
        m_isDirty = false;
    }
}
