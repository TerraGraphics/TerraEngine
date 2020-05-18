#include "preview_window.h"

#include <DiligentCore/Graphics/GraphicsEngine/interface/SwapChain.h>
#include <DiligentCore/Graphics/GraphicsEngine/interface/DeviceCaps.h>

#include "core/dg/dg.h"
#include "core/engine.h"
#include "preview_scene.h"
#include "core/dg/device.h"
#include "core/math/types.h"
#include "core/camera/camera.h"
#include "core/math/constants.h"
#include "core/dg/graphics_types.h"
#include "middleware/imgui/imgui.h"
#include "middleware/imgui/widgets.h"
#include "core/material/vdecl_item.h"
#include "middleware/gizmo/gizmo_3d.h"
#include "core/render/render_target.h"
#include "core/material/vdecl_storage.h"
#include "middleware/imgui/imgui_math.h"
#include "core/material/material_builder.h"
#include "middleware/std_render/std_scene.h"
#include "middleware/camera/editor_controller.h"


class TransformNode;

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
}

void PreviewWindow::Create() {
    auto& engine = Engine::Get();
    auto& device = engine.GetDevice();
    m_isOpenGL = device->GetDeviceCaps().IsGLDevice();

    ColorTargetDesc colorTargets[] = {
        ColorTargetDesc(dg::TEX_FORMAT_RGBA8_UNORM, math::Color4f(1.f), "rt::color::preview"),
        ColorTargetDesc(dg::TEX_FORMAT_RGBA8_UNORM, math::Color4f(1.f), "rt::color::picker") };

    m_renderTarget->Create(device, RenderTargetDesc(
        _countof(colorTargets), colorTargets,
        DepthTargetDesc(engine.GetSwapChain()->GetDesc().DepthBufferFormat, "rt::depth::preview"),
        CPUTargetDesc(1, 1, 1, "rt::color::cpu")
    ));

    const auto vDeclIdPerInstance = engine.GetVDeclStorage()->Add({
        VDeclItem("WorldRow0", VDeclType::Float4, 1, false),
        VDeclItem("WorldRow1", VDeclType::Float4, 1, false),
        VDeclItem("WorldRow2", VDeclType::Float4, 1, false),
        VDeclItem("WorldRow3", VDeclType::Float4, 1, false),
        VDeclItem("NormalRow0", VDeclType::Float3, 1, false),
        VDeclItem("NormalRow1", VDeclType::Float3, 1, false),
        VDeclItem("NormalRow2", VDeclType::Float3, 1, false),
        VDeclItem("IdColor", VDeclType::Color4, 1, false),
    });
    auto scene = std::make_shared<StdScene>(vDeclIdPerInstance, true);
    m_camera = scene->GetCamera();
    m_camera->SetViewParams(dg::float3(-10, 2, 0), dg::float3(1, 0, 0));
    m_controller->SetCamera(m_camera);

    m_scene->Create(scene);
    m_scene->AddChild(m_gizmo->Create());
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
    if (m_draw) {
        m_renderTarget->Bind(Engine::Get().GetContext());
        m_scene->Draw();
    }
}
