#include "editor/editor_scene_controller.h"

#include <cstddef>

#include "dg/dg.h"
#include "imgui/imgui.h"
#include "imgui/internal.h"
#include "core/math/types.h"
#include "middleware/imgui/gui.h"
#include "editor/windows/panel_window.h"
#include "editor/windows/scene_window.h"
#include "editor/windows/gschema_window.h"
#include "middleware/std_render/std_scene.h"


EditorSceneController::EditorSceneController()
    : m_scene(new StdScene())
    , m_sceneWindow(new SceneWindow())
    , m_gsSchemaWindow(new GSchemaWindow())
    , m_propertyPanel(new PanelWindow()) {

}

EditorSceneController::~EditorSceneController() {
    m_scene.reset();
    m_sceneWindow.reset();
    m_gsSchemaWindow.reset();
    m_propertyPanel.reset();
}

void EditorSceneController::Create(const std::shared_ptr<gui::Gui>& gui) {
    m_gui = gui;
    m_propertyPanel->Create();
    m_gsSchemaWindow->Create(m_propertyPanel);
    m_scene->Create(false, dg::TEXTURE_FORMAT(0), math::Color4f(1.f));
    m_sceneWindow->Create();
}

void EditorSceneController::Update(double deltaTime) {
    m_gui->StartFrame();
    DockSpace();
    m_sceneWindow->Update(deltaTime);
    m_gsSchemaWindow->Draw();

    // draw after all
    m_propertyPanel->Draw();
    FooterWindow();
    // ImGui::ShowDemoWindow(nullptr);

    m_scene->Update();
}

void EditorSceneController::Draw() {
    m_sceneWindow->Draw();
    m_scene->Draw();
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
        ImGui::DockBuilderDockWindow("gseditor", dockCentral);
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
