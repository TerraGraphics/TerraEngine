#include "core/scene/scene.h"

#include <DiligentCore/Graphics/GraphicsEngine/interface/DeviceContext.h>
#include <DiligentCore/Graphics/GraphicsTools/interface/GraphicsUtilities.h>

#include "core/scene/material_node.h"
#include "core/scene/vertex_buffer.h"


Scene::Scene(DevicePtr device, ContextPtr context)
    : m_device(device)
    , m_context(context) {

}

void Scene::Update() {
    m_nodeListForDraw.clear();
    UpdateGraph(m_nodeListForDraw);

    if (!m_transformBuffer) {
        m_transformBuffer = std::make_shared<WriteableVertexBuffer>(m_device, m_nodeListForDraw.size() * (sizeof(dg::float4x4) + sizeof(dg::float3x3)), dg::USAGE_STAGING, "transform vb");
    }

    uint32_t ind = 0;
    float* data = m_transformBuffer->Map<float>(m_context);
    for (const auto& node: m_nodeListForDraw) {
        auto* data4x4 = reinterpret_cast<dg::float4x4*>(data);
        *data4x4 = node->GetWorldMatrix();
        data += 16;
        auto* data3x3 = reinterpret_cast<dg::float3x3*>(data);
        *data3x3 = node->GetNormalMatrix();
        data += 9;
    }
    m_transformBuffer->Unmap(m_context);
}

uint32_t Scene::Draw() {
    uint32_t primitiveCount = 0;

    uint32_t ind = 0;
    m_transformBuffer->BindExclusively(m_context, 1);
    for (const auto& node: m_nodeListForDraw) {
        auto materialNode = node->GetMaterialNode();
        std::shared_ptr<GeometryNode> geometry = materialNode->GetGeometry();
        materialNode->Bind(m_device, m_context);
        primitiveCount += materialNode->Draw(m_context, ind);
        ++ind;
    }

    return primitiveCount;
}
