#include "core/camera/camera.h"


Camera::Camera(float fovy, float nearPlane, float farPlane, bool isGL)
	: m_fovy(fovy)
	, m_nearPlane(nearPlane)
	, m_farPlane(farPlane)
	, m_isGL(isGL) {
	m_matProj = dg::float4x4::Projection(m_fovy, m_aspectRatio, m_nearPlane, m_farPlane, m_isGL);

	calcViewMatrix(dg::float3(1.0, 0.0, 0.0));
}

void Camera::SetAspectRatio(float value) {
	m_aspectRatio = value;
	m_matProj = dg::float4x4::Projection(m_fovy, m_aspectRatio, m_nearPlane, m_farPlane, m_isGL);
}

void Camera::SetNearPlane(float value) {
	m_nearPlane = value;
	m_matProj = dg::float4x4::Projection(m_fovy, m_aspectRatio, m_nearPlane, m_farPlane, m_isGL);
}

void Camera::SetFarPlane(float value) {
	m_farPlane = value;
	m_matProj = dg::float4x4::Projection(m_fovy, m_aspectRatio, m_nearPlane, m_farPlane, m_isGL);
}

void Camera::SetViewParams(const dg::float3& position, const dg::float3& direction) {
	m_position = position;
	calcViewMatrix(direction);
}

void Camera::calcViewMatrix(const dg::float3& dir) {
	// see glm::lookAtLH
	const dg::float3 up(0.0, 1.0, 0.0);
	dg::float3 const direction(dg::normalize(dir));
	dg::float3 const lvec(dg::normalize(dg::cross(up, direction)));
	dg::float3 const u(dg::cross(direction, lvec));

	m_matView[0][0] = lvec.x;
	m_matView[1][0] = lvec.y;
	m_matView[2][0] = lvec.z;

	m_matView[0][1] = u.x;
	m_matView[1][1] = u.y;
	m_matView[2][1] = u.z;

	m_matView[0][2] = direction.x;
	m_matView[1][2] = direction.y;
	m_matView[2][2] = direction.z;

	m_matView[3][0] =-dg::dot(lvec, m_position);
	m_matView[3][1] =-dg::dot(u, m_position);
	m_matView[3][2] =-dg::dot(direction, m_position);
}
