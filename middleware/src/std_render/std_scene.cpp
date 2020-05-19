#include "middleware/std_render/std_scene.h"

#include <vector>
#include <cstddef>

#include "core/dg/dg.h"
#include "core/engine.h"
#include "core/dg/device.h"
#include "core/dg/context.h" // IWYU pragma: keep
#include "core/camera/camera.h"
#include "core/math/constants.h"
#include "core/scene/vertex_buffer.h"
#include "core/render/render_target.h"
#include "core/scene/transform_graph.h"
#include "core/material/material_builder.h"


StdScene::StdScene(uint16_t vDeclIdPerInstance, bool addId)
    : Scene(vDeclIdPerInstance)
    , m_addId(addId)
    , m_renderTarget(std::make_unique<RenderTarget>()) {

}

StdScene::~StdScene() {
    m_renderTarget.reset();
}

TextureViewPtr StdScene::GetColorTexture() {
    return m_renderTarget->GetColorTexture(0);
}

void StdScene::StartSearchingNodeInPoint(uint32_t x, uint32_t y) {
    m_pickerRect = math::Rect(x, y, 1, 1);
    m_pickerState = PickerState::NeedDraw;
    m_pickerResult.reset();
}

bool StdScene::GetNodeInPoint(std::shared_ptr<TransformNode>& node) {
    if (m_pickerState == PickerState::Finish) {
        node = m_pickerResult;
        return true;
    }

    return false;
}

void StdScene::Create(bool renderToTexture, dg::TEXTURE_FORMAT format, math::Color4f clearColor, uint32_t width, uint32_t height) {
    auto& engine = Engine::Get();
    auto& device = engine.GetDevice();
    auto& materialBuilder = engine.GetMaterialBuilder();

    m_renderTarget->Create(device, engine.GetContext(), clearColor, width, height);
    if (renderToTexture) {
        m_renderTarget->SetColorTarget(0, format, clearColor, "rt::color::main");
        m_renderTarget->SetColorTarget(1, dg::TEX_FORMAT_RGBA8_UNORM, math::Color4f(1.f), "rt::color::picker");
        m_renderTarget->SetDepthTarget(engine.GetSwapChain()->GetDesc().DepthBufferFormat, "rt::depth::main");
    } else {
        m_renderTarget->SetDefaultColorTarget(0, clearColor);
        m_renderTarget->SetDefaultDepthTarget();
    }

    m_camera = std::make_shared<Camera>(QuarterPI<float>(), 0.1f, 100.0f, device->GetDeviceCaps().IsGLDevice(), true);
    m_vsCameraVarId = materialBuilder->AddGlobalVar<dg::ShaderCamera>(dg::SHADER_TYPE::SHADER_TYPE_VERTEX, "Camera");
    m_psCameraVarId = materialBuilder->AddGlobalVar<dg::ShaderCamera>(dg::SHADER_TYPE::SHADER_TYPE_PIXEL, "Camera");
    m_gsCameraVarId = materialBuilder->AddGlobalVar<dg::ShaderCamera>(dg::SHADER_TYPE::SHADER_TYPE_GEOMETRY, "Camera");
}

void StdScene::Update(uint32_t width, uint32_t height) {
    auto& engine = Engine::Get();
    auto& device = engine.GetDevice();
    auto& context = engine.GetContext();

    m_shaderCamera.matViewProj = m_camera->GetViewMatrix() * m_camera->GetProjMatrix();
    m_shaderCamera.vecPosition = dg::float4(m_camera->GetPosition(), 0);
    m_shaderCamera.vecViewDirection = dg::float4(m_camera->GetDirection(), 0);

    uint32_t findNodeId = 0;
    if (m_pickerState == PickerState::WaitResult) {
        if (!m_renderTarget->ReadCPUTarget(findNodeId)) {
            findNodeId = 0;
        } else {
            m_pickerState = PickerState::Finish;
        }
    }
    TransformUpdateDesc& updateDesc = Scene::Update(findNodeId);
    if (findNodeId != 0) {
        m_pickerResult = updateDesc.findResult;
        updateDesc.findResult.reset();
    }

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
    if (m_pickerState == PickerState::NeedDraw) {
        m_renderTarget->Update(engine.GetSwapChain(), 2, width, height);
        m_pickerState = PickerState::NeedCopy;
    } else if (m_pickerState == PickerState::WaitResult) {
        m_renderTarget->Update(engine.GetSwapChain(), 2, width, height);
    } else {
        m_renderTarget->Update(engine.GetSwapChain(), 1, width, height);
    }
}

uint32_t StdScene::Draw() {
    auto& engine = Engine::Get();
    auto& context = engine.GetContext();
    auto& builder = engine.GetMaterialBuilder();

    m_renderTarget->Bind();
    builder->UpdateGlobalVar(m_vsCameraVarId, m_shaderCamera);
    builder->UpdateGlobalVar(m_psCameraVarId, m_shaderCamera);
    builder->UpdateGlobalVar(m_gsCameraVarId, m_shaderCamera);
    m_transformBuffer->BindExclusively(context, 1);
    uint32_t result = Scene::Draw(context);
    if (m_pickerState == PickerState::NeedCopy) {
        m_renderTarget->CopyColorTarget(1, m_pickerRect);
        m_pickerState = PickerState::WaitResult;
    }

    return result;
}