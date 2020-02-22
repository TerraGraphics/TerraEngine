#include "editor_scene_controller.h"

#include "middleware/imgui/imgui_math.h"

#include <imgui_internal.h>

#include "core/engine.h"
#include "preview_scene.h"
#include "preview_window.h"
#include "middleware/imgui/gui.h"
#include "core/dg/render_device.h"
#include "core/dg/device_context.h"
#include "middleware/imgui/widgets.h"
#include "core/render/render_target.h"
#include "core/material/material_builder.h"
#include "middleware/camera/editor_controller.h"


EditorSceneController::EditorSceneController()
    : m_controller(new EditorCameraController())
    , m_previewScene(new PreviewScene())
    , m_sceneRenderTarget(new RenderTarget())
    , m_previewRenderTarget(new RenderTarget())
    , m_previewWindow(new PreviewWindow()) {

}

EditorSceneController::~EditorSceneController() {
    if (m_controller != nullptr) {
        delete m_controller;
    }

    m_previewScene.reset();
    m_sceneRenderTarget.reset();
    m_previewRenderTarget.reset();
    m_previewWindow.reset();
}

void EditorSceneController::Create(uint32_t vsCameraVarId, uint32_t psCameraVarId, uint32_t gsCameraVarId, const std::shared_ptr<Gui>& gui) {
    auto& engine = Engine::Get();
    auto device = engine.GetDevice();
    auto& scDesc = engine.GetSwapChain()->GetDesc();

    m_vsCameraVarId = vsCameraVarId;
    m_psCameraVarId = psCameraVarId;
    m_gsCameraVarId = gsCameraVarId;
    m_isOpenGL = device->GetDeviceCaps().IsGLDevice();
    m_gui = gui;

    m_camera = std::make_shared<Camera>(QuarterPI<float>(), 0.1f, 100.0f, m_isOpenGL, true);
    m_camera->SetViewParams(dg::float3(-10, 2, 0), dg::float3(1, 0, 0));
    m_controller->SetCamera(m_camera);

    m_previewScene->Create();

    m_sceneRenderTarget->Create(device, math::Color4f(1.f));

    ColorTargetDesc colorTargets[] = {
        ColorTargetDesc(dg::TEX_FORMAT_RGBA8_UNORM, math::Color4f(1.f), "rt::color::preview"),
        ColorTargetDesc(dg::TEX_FORMAT_RGBA8_UNORM, math::Color4f(1.f), "rt::color::picker") };
    m_previewRenderTarget->Create(device, RenderTargetDesc(
        _countof(colorTargets), colorTargets,
        DepthTargetDesc(scDesc.DepthBufferFormat, "rt::depth::preview"),
        CPUTargetDesc(1, 1, 1, "rt::color::cpu")
        ));

    m_previewWindow->Create();
}

void EditorSceneController::Update(double deltaTime) {
    auto& engine = Engine::Get();
    auto& handler = engine.GetEventHandler();
    auto& swapChain = engine.GetSwapChain();

    m_controller->Update(handler, m_viewWidht, m_viewHeight, static_cast<float>(deltaTime));
    m_shaderCamera.matViewProj = m_camera->GetViewMatrix() * m_camera->GetProjMatrix();
    m_shaderCamera.vecPosition = dg::float4(m_camera->GetPosition(), 0);
    m_shaderCamera.vecViewDirection = dg::float4(m_camera->GetDirection(), 0);

    m_gui->StartFrame();
    DockSpace();
    m_previewWindow->Update(deltaTime);
    ViewWindow();
    PropertyWindow();
    FooterWindow();
    // ImGui::ShowDemoWindow(nullptr);

    if (m_clicked) {
        if (m_previewRenderTarget->ReadCPUTarget(engine.GetImmediateContext(), m_value)) {
            m_previewScene->SelectNode(m_value);
            m_clicked = false;
        }
    }

    m_previewScene->Update(deltaTime, m_camera);
    m_sceneRenderTarget->Update(swapChain);
    m_previewRenderTarget->Update(swapChain, m_viewWidht, m_viewHeight);
}

void EditorSceneController::Draw() {
    auto& engine = Engine::Get();
    auto& context = engine.GetImmediateContext();
    auto& builder = engine.GetMaterialBuilder();

    m_previewRenderTarget->Bind(context);
    builder->UpdateGlobalVar(m_vsCameraVarId, m_shaderCamera);
    builder->UpdateGlobalVar(m_psCameraVarId, m_shaderCamera);
    builder->UpdateGlobalVar(m_gsCameraVarId, m_shaderCamera);

    m_previewScene->Draw();
    m_previewWindow->Draw();

    m_sceneRenderTarget->Bind(context);

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

        math::Rectf rc = Image(m_previewRenderTarget->GetColorTexture(0), math::Size(m_viewWidht, m_viewHeight), m_isOpenGL);
        if (ImGui::IsWindowHovered() &&
            ImGui::IsMouseHoveringRect(ToImGui(rc.Min()), ToImGui(rc.Max()))) {

            auto x = static_cast<uint32_t>(ImGui::GetIO().MousePos.x - rc.x);
            auto y = static_cast<uint32_t>(ImGui::GetIO().MousePos.y - rc.y);

            auto rayDir = m_camera->ScreenPointToRay(math::Point(x, y), rc.SizeCast<uint32_t>());
            m_previewScene->SetMouseRay(m_camera->GetPosition(), rayDir);

            if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                m_clicked = true;
                m_clickPos.x = x;
                m_clickPos.y = y;
                m_previewScene->SetSpherePos(m_camera->GetPosition() + rayDir * 2.f);

                m_previewRenderTarget->CopyColorTarget(Engine::Get().GetImmediateContext(),
                    m_clickPos.x, m_isOpenGL ? static_cast<uint32_t>(rc.Height()) - m_clickPos.y : m_clickPos.y);
            }
        }

        ImGui::End();
    }
}

void EditorSceneController::PropertyWindow() {
    bool* pOpen = nullptr;
    ImGuiWindowFlags windowFlags = 0;
    if (ImGui::Begin("Property", pOpen, windowFlags)) {
        math::Color4 crl;
        crl.value = m_value;
        std::string str = fmt::format("r={} g={} b={} a={}", crl.red, crl.green, crl.blue, crl.alpha);
        ImGui::Text("pos: %d %d = %s (%d)", m_clickPos.x, m_clickPos.y, str.c_str(), m_value);
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
