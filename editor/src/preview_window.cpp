#include "preview_window.h"

#include <DiligentCore/Graphics/GraphicsEngine/interface/SwapChain.h>

#include "core/engine.h"
#include "preview_scene.h"
#include "core/dg/render_device.h"
#include "middleware/imgui/widgets.h"
#include "core/render/render_target.h"
#include "middleware/imgui/imgui_math.h"
#include "core/material/material_builder.h"
#include "middleware/camera/editor_controller.h"


PreviewWindow::PreviewWindow()
    : m_scene(new PreviewScene())
    , m_renderTarget(new RenderTarget())
    , m_controller(new EditorCameraController()) {

}

PreviewWindow::~PreviewWindow() {
    m_controller.reset();
    m_renderTarget.reset();
    m_scene.reset();
    m_camera.reset();
}

void PreviewWindow::Create(uint32_t vsCameraVarId, uint32_t psCameraVarId, uint32_t gsCameraVarId) {
    auto& engine = Engine::Get();
    auto& device = engine.GetDevice();

    m_vsCameraVarId = vsCameraVarId;
    m_psCameraVarId = psCameraVarId;
    m_gsCameraVarId = gsCameraVarId;
    m_isOpenGL = device->GetDeviceCaps().IsGLDevice();

    m_camera = std::make_shared<Camera>(QuarterPI<float>(), 0.1f, 100.0f, m_isOpenGL, true);
    m_camera->SetViewParams(dg::float3(-10, 2, 0), dg::float3(1, 0, 0));
    m_controller->SetCamera(m_camera);

    ColorTargetDesc colorTargets[] = {
        ColorTargetDesc(dg::TEX_FORMAT_RGBA8_UNORM, math::Color4f(1.f), "rt::color::preview"),
        ColorTargetDesc(dg::TEX_FORMAT_RGBA8_UNORM, math::Color4f(1.f), "rt::color::picker") };

    m_renderTarget->Create(device, RenderTargetDesc(
        _countof(colorTargets), colorTargets,
        DepthTargetDesc(engine.GetSwapChain()->GetDesc().DepthBufferFormat, "rt::depth::preview"),
        CPUTargetDesc(1, 1, 1, "rt::color::cpu")
    ));

    m_scene->Create();
}

void PreviewWindow::Update(double deltaTime) {
    auto& engine = Engine::Get();
    auto& handler = engine.GetEventHandler();

    bool* pOpen = nullptr;
    ImGuiWindowFlags windowFlags = 0;
    if (ImGui::Begin("preview", pOpen, windowFlags)) {
        m_draw = true;
        auto size = ImGui::GetContentRegionAvail();
        auto uSize = math::Size(static_cast<uint32_t>(size.x), static_cast<uint32_t>(size.y));
        math::Rectf rc = Image(m_renderTarget->GetColorTexture(0), uSize, m_isOpenGL);

        m_controller->Update(handler, static_cast<uint32_t>(rc.Width()), static_cast<uint32_t>(rc.Height()), static_cast<float>(deltaTime));
        m_shaderCamera.matViewProj = m_camera->GetViewMatrix() * m_camera->GetProjMatrix();
        m_shaderCamera.vecPosition = dg::float4(m_camera->GetPosition(), 0);
        m_shaderCamera.vecViewDirection = dg::float4(m_camera->GetDirection(), 0);

        if (ImGui::IsWindowHovered() &&
            ImGui::IsMouseHoveringRect(ToImGui(rc.Min()), ToImGui(rc.Max()))) {

            auto x = static_cast<uint32_t>(ImGui::GetIO().MousePos.x - rc.x);
            auto y = static_cast<uint32_t>(ImGui::GetIO().MousePos.y - rc.y);

            auto rayDir = m_camera->ScreenPointToRay(math::Point(x, y), rc.SizeCast<uint32_t>());
            m_scene->SetMouseRay(m_camera->GetPosition(), rayDir);

            if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                m_clicked = true;
                m_scene->SetSpherePos(m_camera->GetPosition() + rayDir * 2.f);

                m_renderTarget->CopyColorTarget(Engine::Get().GetImmediateContext(),
                    x, m_isOpenGL ? static_cast<uint32_t>(rc.Height()) - y : y);
            }
        }

        if (m_clicked) {
            uint32_t value = 0;
            if (m_renderTarget->ReadCPUTarget(engine.GetImmediateContext(), value)) {
                m_scene->SelectNode(value);
                m_clicked = false;
            }
        }

        m_scene->Update(deltaTime, m_camera);
        m_renderTarget->Update(engine.GetSwapChain(), static_cast<uint32_t>(rc.Width()), static_cast<uint32_t>(rc.Height()));

        ImGui::End();
    } else {
        m_draw = false;
    }
}

void PreviewWindow::Draw() {
    auto& engine = Engine::Get();
    auto& context = engine.GetImmediateContext();
    auto& builder = engine.GetMaterialBuilder();

    if (m_draw) {
        m_renderTarget->Bind(context);
        builder->UpdateGlobalVar(m_vsCameraVarId, m_shaderCamera);
        builder->UpdateGlobalVar(m_psCameraVarId, m_shaderCamera);
        builder->UpdateGlobalVar(m_gsCameraVarId, m_shaderCamera);

        m_scene->Draw();
    }
}
