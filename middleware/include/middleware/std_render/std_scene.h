#pragma once

#include <memory>
#include <cstdint>

#include "core/dg/dg.h"
#include "core/math/types.h"
#include "core/scene/scene.h"
#include "middleware/std_render/structures.h"


class Camera;
class RenderTarget;
class TransformNode;
class WriteableVertexBuffer;
class StdScene : public Scene {
    enum class PickerState : uint8_t {
        NeedDraw,
        NeedCopy,
        WaitResult,
        Finish,
    };
public:
    StdScene();
    ~StdScene();

public:
    std::shared_ptr<Camera>& GetCamera() noexcept { return m_camera; }
    void SetCamera(const std::shared_ptr<Camera>& camera) { m_camera = camera; }
    TextureViewPtr GetColorTexture();

    void StartSearchingNodeInPoint(uint32_t x, uint32_t y);
    bool GetNodeInPoint(std::shared_ptr<TransformNode>& node);

public:
    void Create(bool renderToTexture, dg::TEXTURE_FORMAT format, math::Color4f clearColor = math::Color4f(1.f), uint32_t width = 1, uint32_t height = 1);
    void Update(uint32_t width = 0, uint32_t height = 0);
    uint32_t Draw();

private:
    math::Rect m_pickerRect;
    PickerState m_pickerState = PickerState::Finish;
    std::shared_ptr<TransformNode> m_pickerResult;
    uint32_t m_vsCameraVarId = 0;
    uint32_t m_psCameraVarId = 0;
    uint32_t m_gsCameraVarId = 0;
    uint16_t m_vDeclIdPerInstance = 0;
    uint16_t m_vDeclIdPerInstancePicker = 0;
    dg::ShaderCamera m_shaderCamera;
    std::shared_ptr<Camera> m_camera;
    std::unique_ptr<RenderTarget> m_renderTarget;
    uint32_t m_transformBufferBufferSize = 0;
    std::shared_ptr<WriteableVertexBuffer> m_transformBuffer;
};
