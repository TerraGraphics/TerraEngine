#pragma once

#include <memory>
#include <DiligentCore/Common/interface/BasicMath.h>

#include "core/common/dg.h"
#include "core/common/ctor.h"

namespace Diligent {
#include "structures.fxh"
}

class Camera;
class GeneralScene;
class FlyCameraController;
class GeneralSceneController : Fixed {
public:
    GeneralSceneController();
    ~GeneralSceneController();

public:
    void Create(uint32_t vsCameraVarId, uint32_t psCameraVarId, uint32_t gsCameraVarId);
    void Update(double deltaTime);
    void Draw();

private:
    uint32_t m_vsCameraVarId = 0;
    uint32_t m_psCameraVarId = 0;
    uint32_t m_gsCameraVarId = 0;

    dg::ShaderCamera m_shaderCamera;
    std::shared_ptr<Camera> m_camera;
    FlyCameraController* m_controller = nullptr;

    GeneralScene* m_generalScene = nullptr;
};
