#include "middleware/std_render/std_scene.h"

#include <vector>
#include <cstddef>

#include <DiligentCore/Graphics/GraphicsEngine/interface/DeviceCaps.h>

#include "core/dg/dg.h"
#include "core/engine.h"
#include "core/dg/device.h"
#include "core/dg/shader.h"
#include "core/dg/context.h" // IWYU pragma: keep
#include "core/camera/camera.h"
#include "core/math/constants.h"
#include "core/dg/graphics_types.h"
#include "core/scene/vertex_buffer.h"
#include "core/scene/transform_graph.h"
#include "core/material/material_builder.h"


StdScene::StdScene(uint16_t vDeclIdPerInstance, bool addId)
    : Scene(vDeclIdPerInstance)
    , m_addId(addId) {

    auto& engine = Engine::Get();
    auto& materialBuilder = engine.GetMaterialBuilder();

    m_camera = std::make_shared<Camera>(QuarterPI<float>(), 0.1f, 100.0f, engine.GetDevice()->GetDeviceCaps().IsGLDevice(), true);
    m_vsCameraVarId = materialBuilder->AddGlobalVar<dg::ShaderCamera>(dg::SHADER_TYPE::SHADER_TYPE_VERTEX, "Camera");
    m_psCameraVarId = materialBuilder->AddGlobalVar<dg::ShaderCamera>(dg::SHADER_TYPE::SHADER_TYPE_PIXEL, "Camera");
    m_gsCameraVarId = materialBuilder->AddGlobalVar<dg::ShaderCamera>(dg::SHADER_TYPE::SHADER_TYPE_GEOMETRY, "Camera");
}

std::shared_ptr<TransformNode> StdScene::Update(uint32_t findId) {
    auto& engine = Engine::Get();
    auto& device = engine.GetDevice();
    auto& context = engine.GetContext();

    m_shaderCamera.matViewProj = m_camera->GetViewMatrix() * m_camera->GetProjMatrix();
    m_shaderCamera.vecPosition = dg::float4(m_camera->GetPosition(), 0);
    m_shaderCamera.vecViewDirection = dg::float4(m_camera->GetDirection(), 0);

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
    auto& engine = Engine::Get();
    auto& context = engine.GetContext();
    auto& builder = engine.GetMaterialBuilder();

    builder->UpdateGlobalVar(m_vsCameraVarId, m_shaderCamera);
    builder->UpdateGlobalVar(m_psCameraVarId, m_shaderCamera);
    builder->UpdateGlobalVar(m_gsCameraVarId, m_shaderCamera);
    m_transformBuffer->BindExclusively(context, 1);
    return Scene::Draw(context);
}
