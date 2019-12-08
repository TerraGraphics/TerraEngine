#include "core/scene/material_node.h"

#include <DiligentCore/Graphics/GraphicsEngine/interface/DeviceContext.h>
#include <DiligentCore/Graphics/GraphicsTools/include/GraphicsUtilities.h>
#include <DiligentCore/Graphics/GraphicsEngine/interface/ShaderResourceBinding.h>

#include "core/common/exception.h"
#include "core/material/material.h"
#include "core/scene/geometry_node.h"


MaterialNode::MaterialNode(const std::shared_ptr<Material>& material, const std::shared_ptr<GeometryNode>& geometry)
    : m_material(material)
    , m_geometry(geometry)
    , m_binding(material->CreateShaderResourceBinding()) {

}

void MaterialNode::SetVertexShaderVar(const dg::Char* name, dg::IDeviceObject* value) {
    auto* var = m_binding->GetVariableByName(dg::SHADER_TYPE_VERTEX, name);
    if (!var) {
        throw EngineError("Unable to find variable '{}' in material {} for vertex shader", name, m_material->GetName());
    }

    var->Set(value);
}

void MaterialNode::SetPixelShaderVar(const dg::Char* name, dg::IDeviceObject* value) {
    auto* var = m_binding->GetVariableByName(dg::SHADER_TYPE_PIXEL, name);
    if (!var) {
        throw EngineError("Unable to find variable '{}' in material {} for pixel shader", name, m_material->GetName());
    }

    var->Set(value);
}

void MaterialNode::Bind(DevicePtr& device, ContextPtr& context) {
    BindVars(device, context);
    m_material->Bind(context);
    context->CommitShaderResources(m_binding, dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
    m_geometry->Bind(context);
}

uint32_t MaterialNode::Draw(ContextPtr& context) {
    return m_geometry->Draw(context);
}

MaterialClrNode::MaterialClrNode(const std::shared_ptr<Material>& material, const std::shared_ptr<GeometryNode>& geometry)
    : MaterialNode(material, geometry) {

}

void MaterialClrNode::BindVars(DevicePtr& device, ContextPtr& context) {
    if (!m_materialCB) {
        dg::CreateUniformBuffer(device, sizeof(dg::ShaderMaterial), "CB::PS::ShaderMaterial", &m_materialCB);
        SetPixelShaderVar("Material", m_materialCB);
        m_isDirty = true;
    }

    if (m_isDirty) {
        m_materialParamsFinish = m_materialParams;
        dg::ShaderMaterial* data;
        context->MapBuffer(m_materialCB, dg::MAP_WRITE, dg::MAP_FLAG_DISCARD, (dg::PVoid&)data);
        *data = m_materialParamsFinish;
        context->UnmapBuffer(m_materialCB, dg::MAP_WRITE);
        m_isDirty = false;
    }
}
