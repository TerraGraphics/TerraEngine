#include "editor_scene_controller.h"

#include <imgui.h>
#include <imgui_internal.h>

#include "core/engine.h"
#include "preview_window.h"
#include "middleware/imgui/gui.h"
#include "core/dg/render_device.h"
#include "core/render/render_target.h"


EditorSceneController::EditorSceneController()
    : m_renderTarget(new RenderTarget())
    , m_previewWindow(new PreviewWindow()) {

}

EditorSceneController::~EditorSceneController() {
    m_renderTarget.reset();
    m_previewWindow.reset();
}

void EditorSceneController::Create(uint32_t vsCameraVarId, uint32_t psCameraVarId, uint32_t gsCameraVarId, const std::shared_ptr<Gui>& gui) {
    auto& engine = Engine::Get();

    m_gui = gui;
    m_renderTarget->Create(engine.GetDevice(), math::Color4f(1.f));
    m_previewWindow->Create(vsCameraVarId, psCameraVarId, gsCameraVarId);
}

void EditorSceneController::Update(double deltaTime) {
    auto& engine = Engine::Get();

    m_gui->StartFrame();
    DockSpace();
    m_previewWindow->Update(deltaTime);
    PropertyWindow();
    FooterWindow();
    // ImGui::ShowDemoWindow(nullptr);

    m_renderTarget->Update(engine.GetSwapChain());
}

void EditorSceneController::Draw() {
    auto& engine = Engine::Get();

    m_previewWindow->Draw();

    m_renderTarget->Bind(engine.GetImmediateContext());
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

        ImGui::DockBuilderDockWindow("preview", dockCentral);
        ImGui::DockBuilderDockWindow("Property", dockRight);
        ImGui::DockBuilderDockWindow("Footer", dockBottom);
        ImGui::DockBuilderFinish(dockspaceID);
    }

    ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
    ImGui::End();
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
