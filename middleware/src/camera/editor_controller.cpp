#include "middleware/camera/editor_controller.h"

#include <cmath>


void EditorCameraController::EnableInput(bool value) noexcept {
    m_enableInput = value;
}

void EditorCameraController::SetCamera(const std::shared_ptr<Camera>& camera) {
    m_camera = camera;
    auto dir = m_camera->IsCoordSystemRH() ? m_directionRH : m_directionLH;
    m_camera->SetViewParams(-dir * m_radius, dir);
    m_camera->SetAspectRatio(m_aspectRatio);
}

void EditorCameraController::Update(const std::shared_ptr<DefaultWindowEventsHandler>& eventHandler, uint32_t width, uint32_t height, float /* deltaTime */) {
    constexpr const auto pitchMax = HalfPI<float>() - QuarterPI<float>() / 45.f;
    constexpr const auto pitchMin = -(HalfPI<float>() - QuarterPI<float>() / 45.f);

    if (m_enableInput) {
        bool update = false;
        // TODO: add settings
        if (eventHandler->IsKeyDown(Key::MouseMiddle)) {
            dg::float2 dtRotation;
            eventHandler->GetCursorOffet(dtRotation.x, dtRotation.y);
            // TODO: add settings
            m_rotation += dtRotation * 0.01f;
            m_rotation.x = std::fmod(m_rotation.x, TwoPI<float>());
            m_rotation.y = dg::clamp(m_rotation.y, pitchMin, pitchMax);

            auto yaw = -m_rotation.x;
            auto pitch = -m_rotation.y;

            m_directionRH.x = std::cos(yaw) * std::cos(pitch);
            m_directionRH.y = std::sin(pitch);
            m_directionRH.z = std::sin(yaw) * std::cos(pitch);
            m_directionRH = -dg::normalize(m_directionRH);
            m_directionLH = dg::float3(m_directionRH.x, m_directionRH.y, -m_directionRH.z);
            update = true;
        }

        auto radiusDt = eventHandler->GetScrollOffset();
        if (std::abs(radiusDt) > 0.1f) {
            // TODO: add settings
            m_radius = dg::clamp(m_radius + radiusDt * -0.1f, 1.f, 100.f);
            update = true;
        }

        if (update) {
            auto dir = m_camera->IsCoordSystemRH() ? m_directionRH : m_directionLH;
            m_camera->SetViewParams(-dir * m_radius, dir);
        }
    }

    if (((m_width != width) || (m_height != height)) && (width != 0) && (height != 0)) {
        m_width = width;
        m_height = height;
        m_aspectRatio = static_cast<float>(width) / static_cast<float>(height);
        m_camera->SetAspectRatio(m_aspectRatio);
    }
}
