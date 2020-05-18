#pragma once

#include <limits>
#include <memory>
#include <cstdint>

#include "core/common/ctor.h"


class Camera;
class Gizmo3D;
class RenderTarget;
class PreviewScene;
class EditorCameraController;
class PreviewWindow : Fixed {
public:
    PreviewWindow();
    ~PreviewWindow();

public:
    void Create();
    void Update(double deltaTime);
    void Draw();

private:
    std::shared_ptr<Camera> m_camera;
    std::unique_ptr<PreviewScene> m_scene;
    std::unique_ptr<RenderTarget> m_renderTarget;
    bool m_waitTextureCopy = false;

    std::unique_ptr<Gizmo3D> m_gizmo;
    uint32_t m_selectedId = std::numeric_limits<uint32_t>::max();

    bool m_draw = false;
    bool m_isOpenGL = false;

    std::unique_ptr<EditorCameraController> m_controller;
};
