#include "editor_scene_controller.h"

#include <imgui.h>
#include <imgui_internal.h>

#include "core/engine.h"
#include "editor_scene.h"
#include "middleware/imgui/gui.h"
#include "core/dg/render_device.h"
#include "core/dg/device_context.h"
#include "core/render/render_target.h"
#include "core/material/material_builder.h"
#include "middleware/camera/editor_controller.h"


EditorSceneController::EditorSceneController()
    : m_controller(new EditorCameraController())
    , m_editorScene(new EditorScene())
    , m_sceneRenderTarget(new RenderTarget())
    , m_previewRenderTarget(new RenderTarget()) {

}

EditorSceneController::~EditorSceneController() {
    if (m_controller != nullptr) {
        delete m_controller;
    }

    if (m_editorScene != nullptr) {
        delete m_editorScene;
    }

    m_sceneRenderTarget.reset();
    m_previewRenderTarget.reset();
}

void EditorSceneController::Create(uint32_t vsCameraVarId, uint32_t psCameraVarId, uint32_t gsCameraVarId, const std::shared_ptr<Gui>& gui) {
    auto& engine = Engine::Get();
    auto device = engine.GetDevice();

    m_vsCameraVarId = vsCameraVarId;
    m_psCameraVarId = psCameraVarId;
    m_gsCameraVarId = gsCameraVarId;
    m_gui = gui;

    m_camera = std::make_shared<Camera>(QuarterPI<float>(), 0.1f, 100.0f, device->GetDeviceCaps().IsGLDevice(), true);
    m_camera->SetViewParams(dg::float3(-10, 2, 0), dg::float3(1, 0, 0));
    m_controller->SetCamera(m_camera);

    m_editorScene->Create();

    m_sceneRenderTarget->Create(device);

    ColorTargetDesc colorTargets[] = { ColorTargetDesc("rt::color::preview"), ColorTargetDesc("rt::color::picker") };
    m_previewRenderTarget->Create(device, RenderTargetDesc(_countof(colorTargets), colorTargets, DepthTargetDesc("rt::depth::preview")));
}

void EditorSceneController::Update(double deltaTime) {
    auto& engine = Engine::Get();
    auto& handler = engine.GetEventHandler();
    auto& swapChain = engine.GetSwapChain();

    m_controller->Update(handler, m_viewWidht, m_viewHeight, static_cast<float>(deltaTime));
    m_shaderCamera.matViewProj = m_camera->GetViewMatrix() * m_camera->GetProjMatrix();
    m_shaderCamera.vecPosition = dg::float4(m_camera->GetPosition(), 0);
    m_shaderCamera.vecViewDirection = dg::float4(m_camera->GetDirection(), 0);

    m_editorScene->Update(deltaTime);
    m_sceneRenderTarget->Update(swapChain);
    m_previewRenderTarget->Update(swapChain, m_viewWidht, m_viewHeight);
}

void EditorSceneController::Draw() {
    auto& engine = Engine::Get();
    auto& context = engine.GetImmediateContext();

    const math::Color4f clearColor(1.f, 1.f, 1.f);
    m_previewRenderTarget->Bind(context, clearColor);

    auto builder = engine.GetMaterialBuilder();
    builder->UpdateGlobalVar(m_vsCameraVarId, m_shaderCamera);
    builder->UpdateGlobalVar(m_psCameraVarId, m_shaderCamera);
    builder->UpdateGlobalVar(m_gsCameraVarId, m_shaderCamera);

    m_editorScene->Draw();

    m_sceneRenderTarget->Bind(context, clearColor);

    m_gui->StartFrame();
    DockSpace();
    ViewWindow();
    PropertyWindow();
    FooterWindow();
    // ImGui::ShowDemoWindow(nullptr);
    m_gui->RenderFrame();
}

void EditorSceneController::DockSpace() {
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

    bool* pOpen = nullptr;
    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking |
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    ImGui::Begin("Root", pOpen, windowFlags);
    ImGui::PopStyleVar(3);

    static bool first = true;
    auto dockspaceID = ImGui::GetID("RootSpace");
    if (first) {
        first = false;
        ImGui::DockBuilderRemoveNode(dockspaceID);
        ImGui::DockBuilderAddNode(dockspaceID, ImGuiDockNodeFlags_None);
        ImGui::DockBuilderSetNodeSize(dockspaceID, viewport->Size);

        ImGuiID dockCentral = dockspaceID;
        ImGuiID dockRight = ImGui::DockBuilderSplitNode(dockCentral, ImGuiDir_Right, 0.20f, NULL, &dockCentral);
        ImGuiID dockBottom = ImGui::DockBuilderSplitNode(dockCentral, ImGuiDir_Down, 0.20f, NULL, &dockCentral);

        ImGui::DockBuilderDockWindow("View", dockCentral);
        ImGui::DockBuilderDockWindow("Property", dockRight);
        ImGui::DockBuilderDockWindow("Footer", dockBottom);
        ImGui::DockBuilderFinish(dockspaceID);
    }

    ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
    ImGui::End();
}

void EditorSceneController::ViewWindow() {
    bool* pOpen = nullptr;
    ImGuiWindowFlags windowFlags = 0;
    if (ImGui::Begin("View", pOpen, windowFlags)) {
        auto size = ImGui::GetContentRegionAvail();
        m_viewWidht = static_cast<uint32_t>(size.x);
        m_viewHeight = static_cast<uint32_t>(size.y);

        ImGui::Image(reinterpret_cast<ImTextureID>(m_previewRenderTarget->GetColorTexture(0).RawPtr()), ImVec2(m_viewWidht, m_viewHeight));
        ImGui::End();
    }
}

void EditorSceneController::PropertyWindow() {
    bool* pOpen = nullptr;
    ImGuiWindowFlags windowFlags = 0;
    if (ImGui::Begin("Property", pOpen, windowFlags)) {
        ImGui::End();
    }
}

void EditorSceneController::FooterWindow() {
    bool* pOpen = nullptr;
    ImGuiWindowFlags windowFlags = 0;
    if (ImGui::Begin("Footer", pOpen, windowFlags)) {
        ImGui::End();
    }
}
