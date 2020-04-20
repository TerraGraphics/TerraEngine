#pragma once

#include <memory>
#include <cstdint>
#include <functional>

#include "core/common/ctor.h"


class Property {
public:
    using Callback = std::function<void ()>;

    Property(Callback&& drawCallback);

    void Draw();

private:
    Callback m_drawCallback;
};

class PropertyWindow : Fixed {
public:
    PropertyWindow();
    ~PropertyWindow();

public:
    void Create();
    void SetProperties(std::initializer_list<Property> properties);
    void Draw();

private:
    std::vector<Property> m_properties;
};
