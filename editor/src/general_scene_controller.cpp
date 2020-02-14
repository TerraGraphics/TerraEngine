#include "general_scene_controller.h"

#include <DiligentCore/Graphics/GraphicsEngine/interface/SwapChain.h>

#include "core/engine.h"
#include "general_scene.h"
#include "core/dg/render_device.h"
#include "core/dg/device_context.h"
#include "core/render/render_target.h"
#include "core/material/material_builder.h"
#include "middleware/camera/fly_controller.h"


GeneralSceneController::GeneralSceneController()
    : m_controller(new FlyCameraController())
    , m_generalScene(new GeneralScene())
    , m_sceneRenderTarget(new RenderTarget()) {

}

GeneralSceneController::~GeneralSceneController() {
    if (m_controller != nullptr) {
        delete m_controller;
    }

    if (m_generalScene != nullptr) {
        delete m_generalScene;
    }

    m_sceneRenderTarget.reset();
}

void GeneralSceneController::Create(uint32_t vsCameraVarId, uint32_t psCameraVarId, uint32_t gsCameraVarId) {
    auto& engine = Engine::Get();
    auto device = engine.GetDevice();

    m_vsCameraVarId = vsCameraVarId;
    m_psCameraVarId = psCameraVarId;
    m_gsCameraVarId = gsCameraVarId;

    m_camera = std::make_shared<Camera>(QuarterPI<float>(), 0.1f, 100.0f, device->GetDeviceCaps().IsGLDevice(), true);
    m_camera->SetViewParams(dg::float3(-10, 2, 0), dg::float3(1, 0, 0));
    m_controller->AttachCamera(m_camera);

    m_generalScene->Create();
    m_sceneRenderTarget->Create(device);
}

void GeneralSceneController::Update(double deltaTime) {
    auto& engine = Engine::Get();
    auto handler = engine.GetEventHandler();
    auto& swapChain = engine.GetSwapChain();
    const auto& desc = swapChain->GetDesc();

    m_controller->Update(handler, desc.Width, desc.Height, static_cast<float>(deltaTime));
    m_shaderCamera.matViewProj = m_camera->GetViewMatrix() * m_camera->GetProjMatrix();
    m_shaderCamera.vecPosition = dg::float4(m_camera->GetPosition(), 0);
    m_shaderCamera.vecViewDirection = dg::float4(m_camera->GetDirection(), 0);

    m_generalScene->Update(deltaTime);
    m_sceneRenderTarget->Update(swapChain);
}

void GeneralSceneController::Draw() {
    auto& engine = Engine::Get();

    m_sceneRenderTarget->Bind(engine.GetImmediateContext(), Color4f(0, 0, 1.f));

    auto builder = engine.GetMaterialBuilder();
    builder->UpdateGlobalVar(m_vsCameraVarId, m_shaderCamera);
    builder->UpdateGlobalVar(m_psCameraVarId, m_shaderCamera);
    builder->UpdateGlobalVar(m_gsCameraVarId, m_shaderCamera);

    m_generalScene->Draw();
}
