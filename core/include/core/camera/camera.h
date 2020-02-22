#pragma once

#include "core/math/basic.h"
#include "core/common/ctor.h"
#include "core/math/constants.h"


class Camera : Fixed {
public:
	Camera() = delete;
	// isCoordSystemRH = true - like OpenGL
	// isCoordSystemRH = false - like Vulkan
	Camera(float fovy, float nearPlane, float farPlane, bool isGL, bool isCoordSystemRH);
	~Camera() = default;

public:
	// screenWidth / screenHeight
	void SetAspectRatio(float value);
	void SetNearPlane(float value);
	void SetFarPlane(float value);
	void SetViewParams(const dg::float3& position, const dg::float3& direction);
	dg::float3 ScreenPointToRay(math::Point mousePos, math::Size screenSize) const;

	float GetNearPlane() const noexcept {
		return m_nearPlane;
	}

	float GetFarPlane() const noexcept {
		return m_farPlane;
	}

	bool IsCoordSystemRH() const noexcept {
		return m_isCoordSystemRH;
	}

	dg::float3 GetPosition() const noexcept {
		return m_position;
	}

	dg::float3 GetDirection() const noexcept {
		return dg::float3(m_matView[0][2], m_matView[1][2], m_matView[2][2]) * (m_isCoordSystemRH ? -1.f : 1.f);
	}

	// A normalized vector that is directed to the left of the direction of the gaze
	dg::float3 GetLeftVector() const noexcept {
		return dg::float3(m_matView[0][0], m_matView[1][0], m_matView[2][0]);
	}

	dg::float3 GetUpDirection() const noexcept {
		return dg::float3(0.0, 1.0, 0.0);
	}

	dg::float4x4 GetProjMatrix() const noexcept {
		return m_matProj;
	}

	dg::float4x4 GetViewMatrix() const noexcept {
		return m_matView;
	}
private:
	void calcProjMatrix();
	void calcViewMatrix(const dg::float3& direction);

private:
	dg::float3 m_position = dg::float3(0, 0, 0);
	// In the radians (45 degrees)
	float m_fovy = QuarterPI<float>();
	// screenWidth / screenHeight
	float m_aspectRatio = 4.0f / 3.0f;
	float m_nearPlane = 0.1f;
	float m_farPlane = 100.0;
	bool m_isGL = false;
	bool m_isCoordSystemRH = true;

	dg::float4x4 m_matProj;
	dg::float4x4 m_matView = dg::One4x4;
};
