#include "editor/windows/preview_window.h"

#include <cstdint>

#include "dg/dg.h"
#include "dg/device.h"
#include "imgui/imgui.h"
#include "core/engine.h"
#include "core/camera/camera.h"
#include "editor/preview_scene.h"
#include "middleware/imgui/image.h"
#include "middleware/gizmo/gizmo_3d.h"
#include "middleware/imgui/imgui_math.h"
#include "middleware/std_render/std_scene.h"
#include "middleware/camera/editor_controller.h"


class TransformNode;

PreviewWindow::PreviewWindow()
    : m_scene(std::make_shared<StdScene>())
    , m_preview(new PreviewScene())
    , m_gizmo(new Gizmo3D())
    , m_controller(new EditorCameraController()) {

}

PreviewWindow::~PreviewWindow() {
    m_controller.reset();
    m_gizmo.reset();
    m_preview.reset();
}

void PreviewWindow::Create() {
    m_isOpenGL = Engine::Get().GetDevice()->GetDeviceCaps().IsGLDevice();

    m_scene->Create(true, dg::TEX_FORMAT_RGBA8_UNORM, math::Color4f(1.f));
    m_camera = m_scene->GetCamera();
    m_camera->SetViewParams(dg::float3(-10, 2, 0), dg::float3(1, 0, 0));
    m_controller->SetCamera(m_camera);

    m_preview->Create(m_scene);
    m_preview->AddChild(m_gizmo->Create());
}

void PreviewWindow::Update(double deltaTime) {
    auto& engine = Engine::Get();
    auto& handler = engine.GetEventHandler();

    bool* pOpen = nullptr;
    ImGuiWindowFlags windowFlags = 0;
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    m_draw = ImGui::Begin("preview", pOpen, windowFlags);
    ImGui::PopStyleVar(1);
    if (m_draw) {
        math::RectF rc;
        gui::ImageStyle style;
        style.margin = math::RectOffsetF();
        style.padding = math::RectOffsetF();
        gui::Image(gui::ToSizeF(ImGui::GetContentRegionAvail()), m_scene->GetColorTexture(), style, &rc);
        auto width = static_cast<uint32_t>(rc.Width());
        auto height = static_cast<uint32_t>(rc.Height());

        bool mouseUnderWindow = (ImGui::IsWindowHovered() &&
                                 ImGui::IsMouseHoveringRect(gui::ToImGui(rc.Min()), gui::ToImGui(rc.Max())));
        m_controller->Update(handler, width, height, static_cast<float>(deltaTime));

        GizmoFoundDesc foundDesc;
        m_gizmo->Update(m_camera, rc, mouseUnderWindow, foundDesc);
        if (foundDesc.needFound) {
            m_scene->StartSearchingNodeInPoint(foundDesc.mouseX, foundDesc.mouseY);
            m_waitTextureCopy = true;
        }

        m_scene->Update(width, height);

        if (m_waitTextureCopy) {
            std::shared_ptr<TransformNode> selectNode;
            if (m_scene->GetNodeInPoint(selectNode)) {
                m_waitTextureCopy = false;
                m_gizmo->SelectNode(selectNode);
            }
        }

        ImGui::End();
    }
}

void PreviewWindow::Draw() {
    if (m_draw) {
        m_scene->Draw();
    }
}
