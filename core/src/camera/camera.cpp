#include "core/camera/camera.h"

#include <cmath>


Camera::Camera(float fovy, float nearPlane, float farPlane, bool isGL, bool isCoordSystemRH)
	: m_fovy(fovy)
	, m_nearPlane(nearPlane)
	, m_farPlane(farPlane)
	, m_isGL(isGL)
	, m_isCoordSystemRH(isCoordSystemRH) {

	calcProjMatrix();
	calcViewMatrix(dg::float3(1.0, 0.0, 0.0));
}

void Camera::SetAspectRatio(float value) {
	m_aspectRatio = value;
	calcProjMatrix();
}

void Camera::SetNearPlane(float value) {
	m_nearPlane = value;
	calcProjMatrix();
}

void Camera::SetFarPlane(float value) {
	m_farPlane = value;
	calcProjMatrix();
}

void Camera::SetViewParams(const dg::float3& position, const dg::float3& direction) {
	m_position = position;
	calcViewMatrix(direction);
}

dg::double3 Camera::ScreenPointToRay(math::PointF mousePos, math::SizeF screenSize) const {
	// see: http://antongerdelan.net/opengl/raycasting.html
	double posX = (2. * static_cast<double>(mousePos.x)) / static_cast<double>(screenSize.w) - 1.;
	double posY = 1. - (2. * static_cast<double>(mousePos.y)) / static_cast<double>(screenSize.h);
	auto rayEye = dg::double4(posX, posY, -1., 1.) * dg::ToMatrix4x4<double>(m_matProj).Inverse();
	rayEye.z = m_isCoordSystemRH ? -1. : 1.;
	rayEye.w = 0;
	dg::double4 ray = dg::ToVector4<double>(rayEye) * dg::ToMatrix4x4<double>(m_matView).Inverse();
	return dg::normalize(dg::double3(ray.x, ray.y, ray.z));
}

// see glm::perspectiveRH_ZO
static dg::float4x4 PerspectiveRH_ZO(float fovy, float aspect, float zNear, float zFar) {
	float const tanHalfFovy = tanf(fovy / 2.f);

	dg::float4x4 m(0.f);
	m.m00 = 1.f / (aspect * tanHalfFovy);
	m.m11 = 1.f / tanHalfFovy;
	m.m22 = zFar / (zNear - zFar);
	m.m23 = -1.f;
	m.m32 = -(zFar * zNear) / (zFar - zNear);
	return m;
}

// see glm::perspectiveRH_NO
static dg::float4x4 PerspectiveRH_NO(float fovy, float aspect, float zNear, float zFar) {
	float const tanHalfFovy = tanf(fovy / 2.f);

	dg::float4x4 m(0.f);
	m.m00 = 1.f / (aspect * tanHalfFovy);
	m.m11 = 1.f / tanHalfFovy;
	m.m22 = - (zFar + zNear) / (zFar - zNear);
	m.m23 = -1.f;
	m.m32 = - (2.f * zFar * zNear) / (zFar - zNear);
	return m;
}

// see glm::perspectiveLH_ZO
static dg::float4x4 PerspectiveLH_ZO(float fovy, float aspect, float zNear, float zFar) {
	float const tanHalfFovy = tanf(fovy / 2.f);

	dg::float4x4 m(0.f);
	m.m00 = 1.f / (aspect * tanHalfFovy);
	m.m11 = 1.f / tanHalfFovy;
	m.m22 = zFar / (zFar - zNear);
	m.m23 = 1.f;
	m.m32 = -(zFar * zNear) / (zFar - zNear);
	return m;
}

// see glm::perspectiveLH_NO
static dg::float4x4 PerspectiveLH_NO(float fovy, float aspect, float zNear, float zFar) {
	float const tanHalfFovy = tanf(fovy / 2.f);

	dg::float4x4 m(0.f);
	m.m00 = 1.f / (aspect * tanHalfFovy);
	m.m11 = 1.f / tanHalfFovy;
	m.m22 = (zFar + zNear) / (zFar - zNear);
	m.m23 = 1.f;
	m.m32 = - (2.f * zFar * zNear) / (zFar - zNear);
	return m;
}

void Camera::calcProjMatrix() {
	if (m_isCoordSystemRH) {
		m_matProj = m_isGL ? PerspectiveRH_NO(m_fovy, m_aspectRatio, m_nearPlane, m_farPlane) : PerspectiveRH_ZO(m_fovy, m_aspectRatio, m_nearPlane, m_farPlane);
	} else {
		m_matProj = m_isGL ? PerspectiveLH_NO(m_fovy, m_aspectRatio, m_nearPlane, m_farPlane) : PerspectiveLH_ZO(m_fovy, m_aspectRatio, m_nearPlane, m_farPlane);
	}
}

void Camera::calcViewMatrix(const dg::float3& dir) {
	// see glm::lookAtLH and glm::lookAtRH
	dg::float3 const direction(dg::normalize(dir) * (m_isCoordSystemRH ? -1.f : 1.f));
	dg::float3 const lvec(dg::normalize(dg::cross(GetUpDirection(), direction)));
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
