#include "core/scene/scene.h"

#include "core/dg/device_context.h"
#include "core/scene/material_node.h"
#include "core/scene/vertex_buffer.h"
#include "core/dg/graphics_utilities.h"


Scene::Scene(DevicePtr device, ContextPtr context, bool addId)
    : m_device(device)
    , m_context(context)
    , m_addId(addId) {

}

void Scene::Update() {
    m_nodeListForDraw.clear();
    UpdateGraph(m_nodeListForDraw);

    if (!m_transformBuffer) {
        size_t itemSize = sizeof(dg::float4x4) + sizeof(dg::float3x3);
        if (m_addId) {
            itemSize += sizeof(uint32_t);
        }
        auto fullSize = m_nodeListForDraw.size() * itemSize;
        m_transformBuffer = std::make_shared<WriteableVertexBuffer>(m_device, fullSize, dg::USAGE_STAGING, "transform vb");
    }

    uint8_t* data = m_transformBuffer->Map<uint8_t>(m_context);
    for (const auto& node: m_nodeListForDraw) {
        *reinterpret_cast<dg::float4x4*>(data) = node->GetWorldMatrix();
        data += sizeof(dg::float4x4);
        *reinterpret_cast<dg::float3x3*>(data) = node->GetNormalMatrix();
        data += sizeof(dg::float3x3);
        if (m_addId) {
            *reinterpret_cast<uint32_t*>(data) = node->GetId();
            data += sizeof(uint32_t);
        }
    }
    m_transformBuffer->Unmap(m_context);
    for (const auto& node: m_nodeListForDraw) {
        node->GetMaterialNode()->Update(m_device, m_context);
    }
}

uint32_t Scene::Draw() {
    uint32_t primitiveCount = 0;

    uint32_t ind = 0;
    m_transformBuffer->BindExclusively(m_context, 1);
    for (const auto& node: m_nodeListForDraw) {
        primitiveCount += node->GetMaterialNode()->Draw(m_context, ind);
        ++ind;
    }

    return primitiveCount;
}
