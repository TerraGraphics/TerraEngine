#pragma once

#include <memory>
#include <cstdint>

#include "core/scene/scene.h"
#include "middleware/std_render/structures.h"


class Camera;
class TransformNode;
class WriteableVertexBuffer;
class StdScene : public Scene {
public:
    StdScene() = delete;
    StdScene(uint16_t vDeclIdPerInstance, bool addId);
    ~StdScene() = default;

public:
    std::shared_ptr<Camera>& GetCamera() noexcept { return m_camera; }
    void SetCamera(const std::shared_ptr<Camera>& camera) { m_camera = camera; }

public:
    std::shared_ptr<TransformNode> Update(uint32_t findId = 0);
    uint32_t Draw();

private:
    bool m_addId = false;
    uint32_t m_vsCameraVarId = 0;
    uint32_t m_psCameraVarId = 0;
    uint32_t m_gsCameraVarId = 0;
    dg::ShaderCamera m_shaderCamera;
    std::shared_ptr<Camera> m_camera;
    uint32_t m_transformBufferBufferElementNumber = 0;
    std::shared_ptr<WriteableVertexBuffer> m_transformBuffer;
};
