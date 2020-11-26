#pragma once

#include <memory>

#include "core/common/ctor.h"


namespace gui {
    class Gui;
}
class StdScene;
class GSchemaWindow;
class SceneWindow;
class PanelWindow;
class EditorSceneController : Fixed {
public:
    EditorSceneController();
    ~EditorSceneController();

public:
    void Create(const std::shared_ptr<gui::Gui>& gui);
    void Update(double deltaTime);
    void Draw();

private:
    void DockSpace();
    void FooterWindow();

private:
    std::shared_ptr<gui::Gui> m_gui = nullptr;
    std::unique_ptr<StdScene> m_scene;
    std::unique_ptr<SceneWindow> m_sceneWindow;
    std::unique_ptr<GSchemaWindow> m_gsSchemaWindow;
    std::shared_ptr<PanelWindow> m_propertyPanel;
};
