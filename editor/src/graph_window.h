#pragma once

#include <memory>

#include "core/common/ctor.h"


class GraphEditor;
class PropertyWindow;
class GraphWindow : Fixed {
public:
    GraphWindow();
    ~GraphWindow();

public:
    void Create(const std::shared_ptr<PropertyWindow>& propertyWindow);
    void Draw();

private:
    std::shared_ptr<GraphEditor> m_graphEditor;
    std::shared_ptr<PropertyWindow> m_propertyWindow;
};
