#pragma once

#include <memory>
#include <cstdint>

#include "core/common/ctor.h"


class GraphWindow : Fixed {
public:
    GraphWindow() = default;
    ~GraphWindow() = default;

public:
    void Create();
    void Update(double deltaTime);
    void Draw();

private:
};
