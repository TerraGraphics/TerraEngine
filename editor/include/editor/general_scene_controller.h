#pragma once

#include "core/common/ctor.h"
#include "middleware/std_render/structures.h"


class GeneralScene;
class FlyCameraController;
class GeneralSceneController : Fixed {
public:
    GeneralSceneController();
    ~GeneralSceneController();

public:
    void Create();
    void Update(double deltaTime);
    void Draw();

private:
    dg::ShaderCamera m_shaderCamera;
    FlyCameraController* m_controller = nullptr;
    GeneralScene* m_generalScene = nullptr;
};
