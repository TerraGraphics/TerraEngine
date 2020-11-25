#pragma once

#include <memory>

#include "core/common/ctor.h"


class Camera;
class Gizmo3D;
class StdScene;
class PreviewScene;
class EditorCameraController;
class SceneWindow : Fixed {
public:
    SceneWindow();
    ~SceneWindow();

public:
    void Create();
    void Update(double deltaTime);
    void Draw();

private:
    std::shared_ptr<Camera> m_camera;
    std::shared_ptr<StdScene> m_scene;
    std::unique_ptr<PreviewScene> m_preview;
    bool m_waitTextureCopy = false;

    std::unique_ptr<Gizmo3D> m_gizmo;

    bool m_draw = false;
    bool m_isOpenGL = false;

    std::unique_ptr<EditorCameraController> m_controller;
};
