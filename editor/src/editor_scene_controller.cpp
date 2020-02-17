#include "editor_scene_controller.h"

#include "middleware/imgui/imgui_math.h"

#include <imgui_internal.h>

#include "core/engine.h"
#include "editor_scene.h"
#include "middleware/imgui/gui.h"
#include "core/dg/render_device.h"
#include "core/dg/device_context.h"
#include "middleware/imgui/widgets.h"
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
    auto& scDesc = engine.GetSwapChain()->GetDesc();

    m_vsCameraVarId = vsCameraVarId;
    m_psCameraVarId = psCameraVarId;
    m_gsCameraVarId = gsCameraVarId;
    m_isOpenGL = device->GetDeviceCaps().IsGLDevice();
    m_gui = gui;

    m_camera = std::make_shared<Camera>(QuarterPI<float>(), 0.1f, 100.0f, m_isOpenGL, true);
    m_camera->SetViewParams(dg::float3(-10, 2, 0), dg::float3(1, 0, 0));
    m_controller->SetCamera(m_camera);

    m_editorScene->Create();

    m_sceneRenderTarget->Create(device);

    ColorTargetDesc colorTargets[] = {
        ColorTargetDesc(scDesc.ColorBufferFormat, "rt::color::preview"),
        ColorTargetDesc(scDesc.ColorBufferFormat, "rt::color::picker") };
    m_previewRenderTarget->Create(device, RenderTargetDesc(_countof(colorTargets), colorTargets,
        DepthTargetDesc(scDesc.DepthBufferFormat, "rt::depth::preview")));

    dg::TextureDesc desc;
    desc.Name = "rt::color::gpu";
    desc.Type = dg::RESOURCE_DIM_TEX_2D;
    desc.Width = 1;
    desc.Height = 1;
    desc.Format = dg::TEX_FORMAT_RGBA8_UNORM;
    desc.MipLevels = 1;
    desc.SampleCount = 1;
    desc.Usage = dg::USAGE_STAGING;
    desc.BindFlags = dg::BIND_NONE;
    desc.CPUAccessFlags = dg::CPU_ACCESS_READ;
    desc.MiscFlags = dg::MISC_TEXTURE_FLAG_NONE;
    desc.CommandQueueMask = 1;
    engine.GetDevice()->CreateTexture(desc, nullptr, &m_rtGpuCopy);
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
    if (m_clicked) {
        m_clicked = false;

        dg::MappedTextureSubresource texData;
        context->MapTextureSubresource(m_rtGpuCopy, 0, 0, dg::MAP_READ, dg::MAP_FLAG_DO_NOT_WAIT, nullptr, texData);
        m_value = *reinterpret_cast<const uint32_t*>(texData.pData);
        context->UnmapTextureSubresource(m_rtGpuCopy, 0, 0);
    }

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

        math::Rectf rc = Image(m_previewRenderTarget->GetColorTexture(0), math::Size(m_viewWidht, m_viewHeight), m_isOpenGL);
        if (ImGui::IsWindowHovered() &&
            ImGui::IsMouseHoveringRect(ToImGui(rc.Min()), ToImGui(rc.Max())) &&
            ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {

            m_clickPos.x = static_cast<uint32_t>(ImGui::GetIO().MousePos.x - rc.x);
            m_clickPos.y = static_cast<uint32_t>(ImGui::GetIO().MousePos.y - rc.y);
            m_clicked = true;

            dg::CopyTextureAttribs cpyAttr;
            cpyAttr.pSrcTexture = m_previewRenderTarget->m_colorTargets[1];
            cpyAttr.SrcMipLevel = 0;
            cpyAttr.SrcSlice = 0;
            dg::Box box;
            box.MinX = m_clickPos.x;
            box.MaxX = m_clickPos.x + 1;
            box.MinY = m_clickPos.y;
            box.MaxY = m_clickPos.y + 1;
            cpyAttr.pSrcBox = &box;
            cpyAttr.SrcTextureTransitionMode = dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION;
            cpyAttr.pDstTexture = m_rtGpuCopy;
            cpyAttr.DstMipLevel = 0;
            cpyAttr.DstSlice = 0;
            cpyAttr.DstX = 0;
            cpyAttr.DstY = 0;
            cpyAttr.DstZ = 0;
            cpyAttr.DstTextureTransitionMode = dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION;

            Engine::Get().GetImmediateContext()->CopyTexture(cpyAttr);
        }

        ImGui::End();
    }
}

void EditorSceneController::PropertyWindow() {
    bool* pOpen = nullptr;
    ImGuiWindowFlags windowFlags = 0;
    if (ImGui::Begin("Property", pOpen, windowFlags)) {
        auto* d = reinterpret_cast<uint8_t*>(&m_value);
        std::string str = fmt::format("{}.{}.{}.{}", d[0], d[1], d[2], d[3]);
        ImGui::Text("pos: %d %d = %s", m_clickPos.x, m_clickPos.y, str.c_str());
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
