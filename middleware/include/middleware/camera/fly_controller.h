#pragma once

#include <memory>
#include <vector>
#include <cstddef>
#include <cstdint>

#include "core/dg/math.h"
#include "core/common/ctor.h"
#include "platforms/default_window_handler.h"


class Camera;
// First-person flight camera controler
class FlyCameraController : Fixed {
public:
    enum class Action : uint8_t {
        Forward = 0,
        Backward = 1,
        Right = 2,
        Left = 3,
        Up = 4,
        Down = 5,
        LowSpeed = 6,
        HighSpeed = 7,
        Last = 8,
    };
    FlyCameraController() = default;
    ~FlyCameraController() = default;

public:
    void EnableInput(bool value) noexcept;
    void SetHotkey(Action action, Key key) noexcept;
    void SetMovementSmoothFactor(float value) noexcept;
    void SetMovementSpeed(float low, float normal, float high) noexcept;
    void SetMouseSmooth(float value) noexcept;
    void SetMouseSensitivity(float value) noexcept;
    void AttachCamera(const std::shared_ptr<Camera>& camera);

    void Update(const std::shared_ptr<DefaultWindowEventsHandler>& eventHandler, uint32_t width, uint32_t height, float deltaTime);

private:
    bool m_enableInput = true;
    float m_movementSmoothFactor = 0.5f;
    float m_movementLowSpeed = 2.0f;
    float m_movementNormalSpeed = 5.0f;
    float m_movementHighSpeed = 15.0f;
    float m_mouseSmoothFactor = 0.5f;
    float m_mouseSensitivity = 10.0f;
    float m_aspectRatio = 4.0f / 3.0f;
    uint32_t m_width = 0;
    uint32_t m_height = 0;

    dg::float3 m_movementSpeed = dg::float3(0, 0, 0);

    // Current camera angle in the radians (x: yaw - left/right,  y: pitch - up/down)
    dg::float2 m_rotation = dg::float2(0, 0);
    dg::float2 m_rotationSpeed = dg::float2(0, 0);
    dg::float3 m_directionRH = dg::float3(1.0f, 0.0f, 0.0f);
    dg::float3 m_directionLH = dg::float3(1.0f, 0.0f, 0.0f);

    Key m_hostkey[static_cast<size_t>(FlyCameraController::Action::Last)] = {
        /*Forward*/Key::W, /*Backward*/Key::S, /*Right*/Key::D, /*Left*/Key::A, /*Up*/Key::E, /*Down*/Key::Q, /*LowSpeed*/Key::Control, /*HighSpeed*/Key::Shift};
    std::vector<std::shared_ptr<Camera>> m_cameras;
};
