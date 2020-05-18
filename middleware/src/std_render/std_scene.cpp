#include "middleware/std_render/std_scene.h"

#include "core/engine.h"
#include "core/dg/context.h" // IWYU pragma: keep
#include "core/scene/vertex_buffer.h"


StdScene::StdScene(uint16_t vDeclIdPerInstance, bool addId)
    : Scene(vDeclIdPerInstance)
    , m_addId(addId) {

}

std::shared_ptr<TransformNode> StdScene::Update(uint32_t findId) {
    auto& engine = Engine::Get();
    auto& device = engine.GetDevice();
    auto& context = engine.GetContext();

    TransformUpdateDesc& updateDesc = Scene::Update(findId);

    auto& nodeList = updateDesc.nodeList;
    if (!m_transformBuffer || (m_transformBufferBufferElementNumber < nodeList.size())) {
        m_transformBufferBufferElementNumber = static_cast<uint32_t>(((nodeList.size() >> 8) + size_t(1)) << size_t(8));
        size_t itemSize = sizeof(dg::float4x4) + sizeof(dg::float3x3);
        if (m_addId) {
            itemSize += sizeof(uint32_t);
        }
        auto fullSize = m_transformBufferBufferElementNumber * itemSize;
        m_transformBuffer = std::make_shared<WriteableVertexBuffer>(device, fullSize, dg::USAGE_STAGING, "transform vb");
    }

    uint8_t* data = m_transformBuffer->Map<uint8_t>(context);
    for (const auto& node: nodeList) {
        *reinterpret_cast<dg::float4x4*>(data) = node.worldMatrix;
        data += sizeof(dg::float4x4);
        *reinterpret_cast<dg::float3x3*>(data) = node.normalMatrix;
        data += sizeof(dg::float3x3);
        if (m_addId) {
            *reinterpret_cast<uint32_t*>(data) = node.id;
            data += sizeof(uint32_t);
        }
    }
    m_transformBuffer->Unmap(context);

    std::shared_ptr<TransformNode> result;
    result.swap(updateDesc.findResult);

    return result;
}

uint32_t StdScene::Draw() {
    auto& context = Engine::Get().GetContext();
    m_transformBuffer->BindExclusively(context, 1);
    return Scene::Draw(context);
}
