#pragma once

#include <memory>

#include "core/common/ctor.h"


namespace gs {
    class Editor;
}
class PropertyWindow;
class GSchemaWindow : Fixed {
public:
    GSchemaWindow();
    ~GSchemaWindow();

public:
    void Create(const std::shared_ptr<PropertyWindow>& propertyWindow);
    void Draw();

private:
    std::shared_ptr<gs::Editor> m_editor;
    std::shared_ptr<PropertyWindow> m_propertyWindow;
};
