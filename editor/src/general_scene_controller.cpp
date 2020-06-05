#include "editor/general_scene_controller.h"

#include <memory>

#include "dg/dg.h"
#include "core/engine.h"
#include "dg/swap_chain.h"
#include "core/math/types.h"
#include "dg/graphics_types.h"
#include "core/camera/camera.h"
#include "editor/general_scene.h"
#include "middleware/std_render/std_scene.h"
#include "middleware/camera/fly_controller.h"


GeneralSceneController::GeneralSceneController()
    : m_controller(new FlyCameraController())
    , m_generalScene(new GeneralScene()) {

}

GeneralSceneController::~GeneralSceneController() {
    if (m_controller != nullptr) {
        delete m_controller;
    }

    if (m_generalScene != nullptr) {
        delete m_generalScene;
    }
}

void GeneralSceneController::Create() {
    auto scene = std::make_shared<StdScene>();
    scene->Create(false, dg::TEXTURE_FORMAT(0), math::Color4f(0, 0, 1.f));
    auto& camera = scene->GetCamera();
    camera->SetViewParams(dg::float3(-10, 2, 0), dg::float3(1, 0, 0));
    m_controller->AttachCamera(camera);

    m_generalScene->Create(scene);
}

void GeneralSceneController::Update(double deltaTime) {
    auto& engine = Engine::Get();
    const auto& desc = engine.GetSwapChain()->GetDesc();

    m_controller->Update(engine.GetEventHandler(), desc.Width, desc.Height, static_cast<float>(deltaTime));
    m_generalScene->Update(deltaTime);
}

void GeneralSceneController::Draw() {
    m_generalScene->Draw();
}
