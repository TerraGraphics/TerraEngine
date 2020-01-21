#include "editor/general_scene_controller.h"

#include <DiligentCore/Graphics/GraphicsEngine/interface/SwapChain.h>
#include <DiligentCore/Graphics/GraphicsEngine/interface/RenderDevice.h>
#include <DiligentCore/Graphics/GraphicsEngine/interface/DeviceContext.h>

#include "core/engine.h"
#include "editor/general_scene.h"
#include "core/material/material_builder.h"
#include "middleware/camera/fly_controller.h"


GeneralSceneController::GeneralSceneController()
    : m_generalScene(new GeneralScene())
    , m_flyController(new FlyCameraController()) {

}

GeneralSceneController::~GeneralSceneController() {
    if (m_flyController != nullptr) {
        delete m_flyController;
    }

    if (m_generalScene != nullptr) {
        delete m_generalScene;
    }
}

void GeneralSceneController::Create(uint32_t vsCameraVarId, uint32_t psCameraVarId, uint32_t gsCameraVarId) {
    auto& engine = Engine::Get();
    auto device = engine.GetDevice();

    uint32_t m_vsCameraVarId = vsCameraVarId;
    uint32_t m_psCameraVarId = psCameraVarId;
    uint32_t m_gsCameraVarId = gsCameraVarId;

    m_camera = std::make_shared<Camera>(QuarterPI<float>(), 0.1f, 100.0f, device->GetDeviceCaps().IsGLDevice(), true);
    m_camera->SetViewParams(dg::float3(-10, 2, 0), dg::float3(1, 0, 0));
    m_flyController->AttachCamera(m_camera);

    m_generalScene->Create();
}

void GeneralSceneController::Update(double deltaTime) {
    auto& engine = Engine::Get();

    auto handler = engine.GetEventHandler();
    const auto& desc = engine.GetSwapChain()->GetDesc();

    m_flyController->Update(handler, desc.Width, desc.Height, deltaTime);
    m_shaderCamera.matViewProj = m_camera->GetViewMatrix() * m_camera->GetProjMatrix();
    m_shaderCamera.vecPosition = dg::float4(m_camera->GetPosition(), 0);
    m_shaderCamera.vecViewDirection = dg::float4(m_camera->GetDirection(), 0);

    m_generalScene->Update(deltaTime);
}

void GeneralSceneController::Draw() {
    auto builder = Engine::Get().GetMaterialBuilder();
    builder->UpdateGlobalVar(m_vsCameraVarId, m_shaderCamera);
    builder->UpdateGlobalVar(m_psCameraVarId, m_shaderCamera);
    builder->UpdateGlobalVar(m_gsCameraVarId, m_shaderCamera);

    m_generalScene->Draw();
}
