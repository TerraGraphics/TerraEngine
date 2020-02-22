#pragma once

#include <memory>

#include "core/math/basic.h"
#include "core/common/ctor.h"

namespace Diligent {
#include "structures.fxh"
}


class Camera;
class RenderTarget;
class PreviewScene;
class EditorCameraController;
class PreviewWindow : Fixed {
public:
    PreviewWindow();
    ~PreviewWindow();

public:
    void Create(uint32_t vsCameraVarId, uint32_t psCameraVarId, uint32_t gsCameraVarId);
    void Update(double deltaTime);
    void Draw();

private:
    std::unique_ptr<PreviewScene> m_scene;
    std::unique_ptr<RenderTarget> m_renderTarget;

    uint32_t m_vsCameraVarId = 0;
    uint32_t m_psCameraVarId = 0;
    uint32_t m_gsCameraVarId = 0;
    bool m_isOpenGL = false;
    bool m_clicked = false;
    bool m_draw = false;

    dg::ShaderCamera m_shaderCamera;
    std::shared_ptr<Camera> m_camera;
    std::unique_ptr<EditorCameraController> m_controller;
};
