#pragma once

#include <memory>

#include "core/common/ctor.h"


namespace gui {
    class Gui;
}
class StdScene;
class GraphWindow;
class GSchemaWindow;
class PreviewWindow;
class PropertyWindow;
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
    std::unique_ptr<GraphWindow> m_graphWindow;
    std::unique_ptr<GSchemaWindow> m_gsShemaWindow;
    std::unique_ptr<PreviewWindow> m_previewWindow;
    std::shared_ptr<PropertyWindow> m_propertyWindow;
};
