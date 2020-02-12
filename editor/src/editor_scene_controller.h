#pragma once

#include <memory>
#include <DiligentCore/Common/interface/BasicMath.h>

#include "core/dg/dg.h"
#include "core/common/ctor.h"

namespace Diligent {
#include "structures.fxh"
}

class Gui;
class Camera;
class Target;
class EditorScene;
class EditorCameraController;
class EditorSceneController : Fixed {
public:
    EditorSceneController();
    ~EditorSceneController();

public:
    void Create(uint32_t vsCameraVarId, uint32_t psCameraVarId, uint32_t gsCameraVarId, const std::shared_ptr<Gui>& gui);
    void Update(double deltaTime);
    void Draw();

private:
    void DockSpace();
    void ViewWindow();
    void PropertyWindow();
    void FooterWindow();

private:
    uint32_t m_vsCameraVarId = 0;
    uint32_t m_psCameraVarId = 0;
    uint32_t m_gsCameraVarId = 0;

    dg::ShaderCamera m_shaderCamera;
    std::shared_ptr<Camera> m_camera;
    EditorCameraController* m_controller = nullptr;

    std::shared_ptr<Gui> m_gui = nullptr;
    EditorScene* m_editorScene = nullptr;
    uint32_t m_viewWidht = 1.f;
    uint32_t m_viewHeight = 1.f;
    std::unique_ptr<Target> m_target;
};
