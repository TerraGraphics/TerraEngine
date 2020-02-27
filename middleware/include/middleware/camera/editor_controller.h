#pragma once

#include <memory>
#include <cstdint>

#include "core/dg/math.h"
#include "core/common/ctor.h"


class Camera;
class DefaultWindowEventsHandler;
// Camera controler for editor
class EditorCameraController : Fixed {
public:
    EditorCameraController() = default;
    ~EditorCameraController() = default;

public:
    void EnableInput(bool value) noexcept;
    void SetCamera(const std::shared_ptr<Camera>& camera);
    void Update(const std::shared_ptr<DefaultWindowEventsHandler>& eventHandler, uint32_t width, uint32_t height, float deltaTime);

private:
    bool m_enableInput = true;
    float m_aspectRatio = 4.0f / 3.0f;
    uint32_t m_width = 0;
    uint32_t m_height = 0;

    // Current camera angle in the radians (x: yaw - left/right,  y: pitch - up/down)
    dg::float2 m_rotation = dg::float2(0, 0);
    float m_radius = 5.f;
    dg::float3 m_directionRH = dg::float3(-1.0f, 0.0f, 0.0f);
    dg::float3 m_directionLH = dg::float3(-1.0f, 0.0f, 0.0f);

    std::shared_ptr<Camera> m_camera;
};
