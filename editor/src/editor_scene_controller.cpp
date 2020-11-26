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
    , m_gschemaWindow(new GSchemaWindow())
    , m_footerPanel(new PanelWindow("Footer"))
    , m_previewPanel(new PanelWindow("Preview"))
    , m_propertyPanel(new PanelWindow("Property")) {

}

EditorSceneController::~EditorSceneController() {
    m_scene.reset();
    m_sceneWindow.reset();
    m_gschemaWindow.reset();
    m_footerPanel.reset();
    m_previewPanel.reset();
    m_propertyPanel.reset();
}

void EditorSceneController::Create(const std::shared_ptr<gui::Gui>& gui) {
    m_gui = gui;

    m_footerPanel->Create();
    m_previewPanel->Create();
    m_propertyPanel->Create();

    m_gschemaWindow->Create(m_previewPanel, m_propertyPanel);
    m_scene->Create(false, dg::TEXTURE_FORMAT(0), math::Color4f(1.f));
    m_sceneWindow->Create();
}

void EditorSceneController::Update(double deltaTime) {
    m_gui->StartFrame();
    DockSpace();
    m_sceneWindow->Update(deltaTime);
    m_gschemaWindow->Draw();

    m_footerPanel->Draw();
    m_previewPanel->Draw();
    m_propertyPanel->Draw();

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

    auto dockspaceID = ImGui::GetID("RootSpace");
    if (m_needInitDockspace) {
        m_needInitDockspace = false;
        ImGui::DockBuilderRemoveNode(dockspaceID);
        ImGui::DockBuilderAddNode(dockspaceID, ImGuiDockNodeFlags_None);
        ImGui::DockBuilderSetNodeSize(dockspaceID, viewport->Size);

        ImGuiID dockCentral = dockspaceID;
        ImGuiID dockLeft, dockLeftTop, dockLeftBottom, dockBottom;
        ImGui::DockBuilderSplitNode(dockCentral, ImGuiDir_Left, 0.20f, &dockLeft, &dockCentral);
        ImGui::DockBuilderSplitNode(dockCentral, ImGuiDir_Down, 0.20f, &dockBottom, &dockCentral);
        ImGui::DockBuilderSplitNode(dockLeft, ImGuiDir_Up, 0.40f, &dockLeftTop, &dockLeftBottom);

        // Main window
        ImGui::DockBuilderDockWindow("GSchema", dockCentral);
        ImGui::DockBuilderDockWindow("Scene", dockCentral);

        // Panel
        ImGui::DockBuilderDockWindow("Footer", dockBottom);
        ImGui::DockBuilderDockWindow("Preview", dockLeftTop);
        ImGui::DockBuilderDockWindow("Property", dockLeftBottom);

        ImGui::DockBuilderFinish(dockspaceID);
    }

    ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
    ImGui::End();
}
