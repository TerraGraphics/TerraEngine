#pragma once

#include <memory>
#include <cstdint>

#include "core/common/ctor.h"


class PropertyWindow : Fixed {
public:
    PropertyWindow();
    ~PropertyWindow();

public:
    void Create();
    void Draw();

private:
};
