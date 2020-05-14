#include "editor_scene_controller.h"

#include <cstddef>

#include "core/engine.h"
#include "graph_window.h"
#include "core/dg/device.h" // IWYU pragma: keep
#include "preview_window.h"
#include "property_window.h"
#include "core/math/types.h"
#include "middleware/imgui/gui.h"
#include "middleware/imgui/imgui.h"
#include "core/render/render_target.h"
#include "middleware/imgui/imgui_internal.h"


EditorSceneController::EditorSceneController()
    : m_graphWindow(new GraphWindow())
    , m_renderTarget(new RenderTarget())
    , m_previewWindow(new PreviewWindow())
    , m_propertyWindow(new PropertyWindow()) {

}

EditorSceneController::~EditorSceneController() {
    m_graphWindow.reset();
    m_renderTarget.reset();
    m_previewWindow.reset();
    m_propertyWindow.reset();
}

void EditorSceneController::Create(uint32_t vsCameraVarId, uint32_t psCameraVarId, uint32_t gsCameraVarId, const std::shared_ptr<gui::Gui>& gui) {
    auto& engine = Engine::Get();

    m_gui = gui;
    m_propertyWindow->Create();
    m_graphWindow->Create(m_propertyWindow);
    m_renderTarget->Create(engine.GetDevice(), math::Color4f(1.f));
    m_previewWindow->Create(vsCameraVarId, psCameraVarId, gsCameraVarId);
}

void EditorSceneController::Update(double deltaTime) {
    auto& engine = Engine::Get();

    m_gui->StartFrame();
    DockSpace();
    m_previewWindow->Update(deltaTime);
    m_graphWindow->Draw();
    m_propertyWindow->Draw();
    FooterWindow();
    // ImGui::ShowDemoWindow(nullptr);

    m_renderTarget->Update(engine.GetSwapChain());
}

void EditorSceneController::Draw() {
    auto& engine = Engine::Get();

    m_previewWindow->Draw();

    m_renderTarget->Bind(engine.GetContext());
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

        ImGui::DockBuilderDockWindow("graph", dockCentral);
        ImGui::DockBuilderDockWindow("preview", dockCentral);
        ImGui::DockBuilderDockWindow("Property", dockRight);
        ImGui::DockBuilderDockWindow("Footer", dockBottom);
        ImGui::DockBuilderFinish(dockspaceID);
    }

    ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
    ImGui::End();
}

void EditorSceneController::FooterWindow() {
    bool* pOpen = nullptr;
    ImGuiWindowFlags windowFlags = 0;
    if (ImGui::Begin("Footer", pOpen, windowFlags)) {
        ImGui::End();
    }
}
