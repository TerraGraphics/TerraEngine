#include "core/scene/scene.h"

#include <vector>
#include <cstddef>

#include "core/dg/math.h"
#include "core/dg/context.h" // IWYU pragma: keep
#include "core/scene/geometry.h"
#include "core/dg/graphics_types.h"
#include "core/scene/material_node.h"
#include "core/scene/vertex_buffer.h"
#include "core/dg/graphics_utilities.h" // IWYU pragma: keep


Scene::Scene(DevicePtr device, ContextPtr context, bool addId)
    : m_device(device)
    , m_context(context)
    , m_addId(addId) {

}

std::shared_ptr<TransformNode> Scene::Update(uint32_t findId) {
    m_updateDesc.nodeList.clear();
    m_updateDesc.findId = findId;
    m_updateDesc.findResult.reset();
    UpdateGraph(m_updateDesc);

    auto& nodeList = m_updateDesc.nodeList;
    if (!m_transformBuffer || (m_transformBufferBufferElementNumber < nodeList.size())) {
        m_transformBufferBufferElementNumber = static_cast<uint32_t>(((nodeList.size() >> 8) + size_t(1)) << size_t(8));
        size_t itemSize = sizeof(dg::float4x4) + sizeof(dg::float3x3);
        if (m_addId) {
            itemSize += sizeof(uint32_t);
        }
        auto fullSize = m_transformBufferBufferElementNumber * itemSize;
        m_transformBuffer = std::make_shared<WriteableVertexBuffer>(m_device, fullSize, dg::USAGE_STAGING, "transform vb");
    }

    uint8_t* data = m_transformBuffer->Map<uint8_t>(m_context);
    for (const auto& node: nodeList) {
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
    for (const auto& node: nodeList) {
        node->GetMaterialNode()->Update(m_device, m_context);
    }

    std::shared_ptr<TransformNode> result;
    result.swap(m_updateDesc.findResult);

    return result;
}

uint32_t Scene::Draw() {
    uint32_t primitiveCount = 0;

    uint32_t ind = 0;
    m_transformBuffer->BindExclusively(m_context, 1);
    for (const auto& node: m_updateDesc.nodeList) {
        node->GetGeometry()->Bind(m_context);
        node->GetMaterialNode()->Bind(m_context);
        primitiveCount += node->GetGeometry()->Draw(m_context, ind);
        ++ind;
    }

    return primitiveCount;
}
