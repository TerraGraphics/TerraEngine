#include "preview_window.h"

#include <imgui.h>
#include <DiligentCore/Graphics/GraphicsEngine/interface/SwapChain.h>
#include <DiligentCore/Graphics/GraphicsEngine/interface/DeviceCaps.h>
#include <DiligentCore/Graphics/GraphicsEngine/interface/InputLayout.h>

#include "core/dg/dg.h"
#include "core/engine.h"
#include "preview_scene.h"
#include "core/dg/device.h"
#include "core/math/types.h"
#include "core/camera/camera.h"
#include "core/math/constants.h"
#include "core/dg/graphics_types.h"
#include "middleware/imgui/widgets.h"
#include "middleware/gizmo/gizmo_3d.h"
#include "core/material/vertex_decl.h"
#include "core/render/render_target.h"
#include "middleware/imgui/imgui_math.h"
#include "core/material/material_builder.h"
#include "middleware/camera/editor_controller.h"

class TransformNode;


// TODO: remove it
static const auto& additionalVDecl = VertexDecl({
        ItemDecl<dg::float4>("WorldRow0", 1, dg::INPUT_ELEMENT_FREQUENCY_PER_INSTANCE),
        ItemDecl<dg::float4>("WorldRow1", 1, dg::INPUT_ELEMENT_FREQUENCY_PER_INSTANCE),
        ItemDecl<dg::float4>("WorldRow2", 1, dg::INPUT_ELEMENT_FREQUENCY_PER_INSTANCE),
        ItemDecl<dg::float4>("WorldRow3", 1, dg::INPUT_ELEMENT_FREQUENCY_PER_INSTANCE),
        ItemDecl<dg::float3>("NormalRow0", 1, dg::INPUT_ELEMENT_FREQUENCY_PER_INSTANCE),
        ItemDecl<dg::float3>("NormalRow1", 1, dg::INPUT_ELEMENT_FREQUENCY_PER_INSTANCE),
        ItemDecl<dg::float3>("NormalRow2", 1, dg::INPUT_ELEMENT_FREQUENCY_PER_INSTANCE),
        ItemDecl<math::Color4>("IdColor", 1, dg::INPUT_ELEMENT_FREQUENCY_PER_INSTANCE),
    });


PreviewWindow::PreviewWindow()
    : m_scene(new PreviewScene())
    , m_renderTarget(new RenderTarget())
    , m_gizmo(new Gizmo3D())
    , m_controller(new EditorCameraController()) {

}

PreviewWindow::~PreviewWindow() {
    m_controller.reset();
    m_gizmo.reset();
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
    m_scene->AddChild(m_gizmo->Create(additionalVDecl));
}

void PreviewWindow::Update(double deltaTime) {
    auto& engine = Engine::Get();
    auto& handler = engine.GetEventHandler();
    auto& context = engine.GetContext();

    bool* pOpen = nullptr;
    ImGuiWindowFlags windowFlags = 0;
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    m_draw = ImGui::Begin("preview", pOpen, windowFlags);
    ImGui::PopStyleVar(1);
    if (m_draw) {
        math::Rect rc = gui::Image(m_renderTarget->GetColorTexture(0), gui::ToSize(ImGui::GetContentRegionAvail()), m_isOpenGL);

        m_controller->Update(handler, rc.Width(), rc.Height(), static_cast<float>(deltaTime));
        m_shaderCamera.matViewProj = m_camera->GetViewMatrix() * m_camera->GetProjMatrix();
        m_shaderCamera.vecPosition = dg::float4(m_camera->GetPosition(), 0);
        m_shaderCamera.vecViewDirection = dg::float4(m_camera->GetDirection(), 0);
        m_renderTarget->Update(engine.GetSwapChain(), rc.Width(), rc.Height());

        bool mouseUnderWindow = (ImGui::IsWindowHovered() &&
                                 ImGui::IsMouseHoveringRect(gui::ToImGui(rc.Min()), gui::ToImGui(rc.Max())));

        bool findSelectedId = false;
        if (m_waitTextureCopy) {
            uint32_t selectedId = 0;
            if (m_renderTarget->ReadCPUTarget(context, selectedId)) {
                m_waitTextureCopy = false;
                if (selectedId != m_selectedId) {
                    m_selectedId = selectedId;
                    if (selectedId == std::numeric_limits<uint32_t>::max()) {
                        m_gizmo->SelectNode(std::shared_ptr<TransformNode>());
                    } else {
                        findSelectedId = true;
                    }
                }
            }
        }

        if (findSelectedId) {
            m_gizmo->SelectNode(m_scene->Update(deltaTime, m_selectedId));
        }

        GizmoFoundDesc foundDesc;
        m_gizmo->Update(m_camera, rc, mouseUnderWindow, foundDesc);
        if (foundDesc.needFound) {
            if (m_isOpenGL) {
                foundDesc.mouseY = foundDesc.windowHeight - foundDesc.mouseY;
            }
            m_renderTarget->CopyColorTarget(context, foundDesc.mouseX, foundDesc.mouseY);
            m_waitTextureCopy = true;
        }

        if (!findSelectedId) {
            m_scene->Update(deltaTime);
        }

        ImGui::End();
    }
}

void PreviewWindow::Draw() {
    auto& engine = Engine::Get();
    auto& builder = engine.GetMaterialBuilder();

    if (m_draw) {
        m_renderTarget->Bind(engine.GetContext());
        builder->UpdateGlobalVar(m_vsCameraVarId, m_shaderCamera);
        builder->UpdateGlobalVar(m_psCameraVarId, m_shaderCamera);
        builder->UpdateGlobalVar(m_gsCameraVarId, m_shaderCamera);

        m_scene->Draw();
    }
}
