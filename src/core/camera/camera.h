#pragma once

#include "core/common/dg.h"
#include "core/common/ctor.h"
#include "core/math/constants.h"


class Camera : Fixed {
public:
	Camera() = delete;
	Camera(float fovy, float nearPlane, float farPlane, bool isGL);
	~Camera() = default;

public:
	// screenWidth / screenHeight
	void SetAspectRatio(float value);
	void SetNearPlane(float value);
	void SetFarPlane(float value);
	void SetViewParams(const dg::float3& position, const dg::float3& direction);

	float GetNearPlane() const noexcept {
		return m_nearPlane;
	}

	float GetFarPlane() const noexcept {
		return m_farPlane;
	}

	dg::float3 GetPosition() const noexcept {
		return m_position;
	}

	dg::float3 GetDirection() const noexcept {
		return dg::float3(m_matView[0][2], m_matView[1][2], m_matView[2][2]);
	}

	dg::float3 GetToEyeDirection() const noexcept {
		return dg::float3(-m_matView[0][2], -m_matView[1][2], -m_matView[2][2]);
	}

	// A normalized vector that is directed to the left of the direction of the gaze
	dg::float3 GetLeftVector() const noexcept { // TODO: add minus?
		return dg::float3(m_matView[0][0], m_matView[1][0], m_matView[2][0]);
	}

	dg::float4x4 GetProjMatrix() const noexcept {
		return m_matProj;
	}

	dg::float4x4 GetViewMatrix() const noexcept {
		return m_matView;
	}
private:
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

	dg::float4x4 m_matProj;
	dg::float4x4 m_matView = dg::One4x4;
};
