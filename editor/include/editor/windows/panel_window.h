#pragma once

#include <string>
#include <vector>
#include <functional>
#include <initializer_list>

#include "core/common/ctor.h"


class Property {
public:
    using Callback = std::function<void ()>;

    Property(Callback&& drawCallback);

    void Draw();

private:
    Callback m_drawCallback;
};

class PanelWindow : Fixed {
public:
    PanelWindow() = delete;
    PanelWindow(const std::string& name);
    ~PanelWindow();

public:
    void Create();
    void SetProperties(std::initializer_list<Property> properties);
    void Draw();

private:
    std::string m_name;
    std::vector<Property> m_properties;
};
