#include "middleware/camera/fly_controller.h"


void FlyCameraController::EnableInput(bool value) noexcept {
    m_enableInput = value;
}

void FlyCameraController::SetHotkey(Action action, Key key) noexcept {
    if (action < Action::Last) {
        m_hostkey[static_cast<size_t>(action)] = key;
    }
}

void FlyCameraController::SetMovementSmoothFactor(float value) noexcept {
    m_movementSmoothFactor = value;
}

void FlyCameraController::SetMovementSpeed(float low, float normal, float high) noexcept {
    m_movementLowSpeed = low;
    m_movementNormalSpeed = normal;
    m_movementHighSpeed = high;
}

void FlyCameraController::SetMouseSmooth(float value) noexcept {
    m_mouseSmoothFactor = value;
}

void FlyCameraController::SetMouseSensitivity(float value) noexcept {
    m_mouseSensitivity = value;
}

void FlyCameraController::AttachCamera(const std::shared_ptr<Camera>& camera) {
    camera->SetAspectRatio(m_aspectRatio);
    camera->SetViewParams(camera->GetPosition(), m_direction);
    m_cameras.push_back(camera);
}

void FlyCameraController::Update(const std::shared_ptr<DefaultWindowEventsHandler>& eventHandler, uint32_t width, uint32_t height, float deltaTime) {
    const auto mouseSensitivityFactor = 1.0f / 2000.0f;
    const auto pitchMax = HalfPI<float>() - 0.2f;
    const auto pitchMin = -(HalfPI<float>() - 0.2f);
    const auto vecEye = dg::float4(1.0f, 0.0f, 0.0f, 1.0f);

    auto positionOffset = dg::float3(0, 0, 0);
    if (m_enableInput) {
        // xxxSmoothFactor - for 60 FPS, recalc for our FPS:
        float smoothFPSCorrection = deltaTime * 60;

        bool isMove = false;
        dg::float3 moveDirection(0, 0, 0);
        if (eventHandler->IsKeyStickyDown(m_hostkey[static_cast<size_t>(Action::Forward)])) {
            moveDirection.x += 1.0f;
            isMove = true;
        } else if (eventHandler->IsKeyStickyDown(m_hostkey[static_cast<size_t>(Action::Backward)])) {
            moveDirection.x -= 1.0f;
            isMove = true;
        }
        if (eventHandler->IsKeyStickyDown(m_hostkey[static_cast<size_t>(Action::Right)])) {
            moveDirection.y += 1.0f;
            isMove = true;
        } else if (eventHandler->IsKeyStickyDown(m_hostkey[static_cast<size_t>(Action::Left)])) {
            moveDirection.y -= 1.0f;
            isMove = true;
        }
        if (isMove) {
            auto currentSpeed = dg::normalize(moveDirection);
            if (eventHandler->IsKeyStickyDown(m_hostkey[static_cast<size_t>(Action::HighSpeed)])) {
                currentSpeed *= m_movementHighSpeed;
            } else if (eventHandler->IsKeyStickyDown(m_hostkey[static_cast<size_t>(Action::LowSpeed)])) {
                currentSpeed *= m_movementLowSpeed;
            } else {
                currentSpeed *= m_movementNormalSpeed;
            }
            m_movementSpeed = dg::lerp(currentSpeed, m_movementSpeed, std::min(m_movementSmoothFactor * smoothFPSCorrection, 0.9f));
            positionOffset = (m_movementSpeed * deltaTime);
        }

        bool isRotate = false;
        dg::float2 dtRotation;
        eventHandler->GetCursorOffet(dtRotation.x, dtRotation.y);
        if (std::abs(dtRotation.x) > 0.1f || std::abs(dtRotation.y) > 0.1f) {
            isRotate = true;
            auto currentRotationSpeed = dtRotation / deltaTime;
            m_rotationSpeed = dg::lerp(currentRotationSpeed, m_rotationSpeed, std::min(m_mouseSmoothFactor * smoothFPSCorrection, 0.9f));
            m_rotation += (m_rotationSpeed * m_mouseSensitivity * mouseSensitivityFactor * deltaTime);

            m_rotation.x = std::fmod(m_rotation.x, TwoPI<float>());
            m_rotation.y = dg::clamp(m_rotation.y, pitchMin, pitchMax);
            m_direction = vecEye * dg::float4x4::RotationZ(m_rotation.y) * dg::float4x4::RotationY(-m_rotation.x);
        }

        if (isMove || isRotate) {
            for(const auto& camera: m_cameras) {
                auto position =
                    camera->GetPosition() +
                    camera->GetDirection() * positionOffset.x +
                    camera->GetLeftVector() * positionOffset.y;
                camera->SetViewParams(position, m_direction);
            }
        }
    }

    if (((m_width != width) || (m_height != height)) && (width != 0) && (height != 0)) {
        m_width = width;
        m_height = height;
        m_aspectRatio = static_cast<float>(width) / static_cast<float>(height);
        for(const auto& camera: m_cameras) {
            camera->SetAspectRatio(m_aspectRatio);
        }
    }
}
