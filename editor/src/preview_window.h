#pragma once

#include "core/common/ctor.h"


class PreviewWindow : Fixed {
public:
    PreviewWindow() = default;
    ~PreviewWindow() = default;

public:
    void Create();
    void Update(double deltaTime);
    void Draw();
};