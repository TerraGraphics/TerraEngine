#pragma once

#include <memory>

#include "core/common/ctor.h"


namespace gs {
    class Editor;
}
class PanelWindow;
class GSchemaWindow : Fixed {
public:
    GSchemaWindow();
    ~GSchemaWindow();

public:
    void Create(const std::shared_ptr<PanelWindow>& previewPanel, const std::shared_ptr<PanelWindow>& propertyPanel);
    void Draw();

private:
    std::shared_ptr<gs::Editor> m_editor;
    std::shared_ptr<PanelWindow> m_previewPanel;
    std::shared_ptr<PanelWindow> m_propertyPanel;
};
