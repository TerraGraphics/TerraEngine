#include "middleware/gizmo/gizmo_shape.h"

#include <cmath>
#include <sys/types.h>

#include "core/math/advanced.h"
#include "middleware/generator/generator.h"
#include "middleware/std_material/std_material.h"


std::shared_ptr<StdMaterial> GizmoArrow::Create(DevicePtr& device, std::shared_ptr<Material>& material, math::Axis axis) {
    m_axis = axis;

    auto axisNum = static_cast<uint>(axis);
    float arrowSpacing = m_height * .05f;
    auto translation = dg::float3(0, 0, 0);
    auto color = dg::float4(0.f, 0.f, 0.f, 1.f);

    float coneHeight = m_height * .1f;
    float coneRadius = coneHeight * .3f;
    ConeShape coneShape({10, 1}, axis, coneRadius, coneHeight);
    translation[axisNum] = m_height - coneHeight * 0.5f;
    coneShape.SetTransform(dg::float4x4::Translation(translation));

    float cylinderHeight = m_height - coneHeight - arrowSpacing;
    CylinderShape cylinderShape({5, 1}, axis, m_radius, cylinderHeight);
    translation[axisNum] = arrowSpacing + cylinderHeight * 0.5f;
    cylinderShape.SetTransform(dg::float4x4::Translation(translation));

    auto node = std::make_shared<StdMaterial>(material, ShapeBuilder(device).Join({&coneShape, &cylinderShape}, "move arrow"));
    color[axisNum] = 1.0f;
    node->SetBaseColor(color);

    return node;
}

bool GizmoArrow::StartMove(dg::float3 rayStart, dg::float3 rayDir) {
    return GetProjection(rayStart, rayDir, m_startMoveCoord);
}

bool GizmoArrow::GetMoveOffset(dg::float3 rayStart, dg::float3 rayDir, dg::float3& offset) const {
    float value;
    if (!GetProjection(rayStart, rayDir, value)) {
        return false;
    }

    offset = dg::float3(0, 0, 0);
    offset[static_cast<uint>(m_axis)] = m_startMoveCoord - value;

    return true;
}

dg::float4x4 GizmoArrow::GetSelectTransform() const {
    auto scale = dg::float3(m_selectScale, m_selectScale, m_selectScale);
    scale[static_cast<uint>(m_axis)] = 1.f;
    return dg::float4x4::Scale(scale);
}

bool GizmoArrow::IsSelected(dg::float3 rayStart, dg::float3 rayDir) const {
    return math::IntersectionRayAndCylinder(rayStart, rayDir, m_axis, m_activeRadius, m_height);
}

/*
    for axis == X: ray (rayStart; rayDir) crosses a plane X0Y (z == 0) or X0Z (y == 0)

    x = x0 + l*t
    y = y0 + m*t
    z = z0 + n*t

    for X0Y: x = x0 + l*t, where t = -z0 / n
    For X0Z: x = x0 + l*t, where t = -y0 / m
*/
bool GizmoArrow::GetProjection(dg::float3 rayStart, dg::float3 rayDir, float& value) const {
    auto ind0 = static_cast<uint>(m_axis);
    auto axis1 = math::Next(m_axis);
    auto ind1 = static_cast<uint>(axis1);
    auto axis2 = math::Next(axis1);
    auto ind2 = static_cast<uint>(axis2);

    if (std::fpclassify(rayDir[ind1]) != FP_ZERO) {
        value = rayStart[ind0] - rayDir[ind0] * rayStart[ind1] / rayDir[ind1];
    } else if (std::fpclassify(rayDir[ind2]) != FP_ZERO) {
        value = rayStart[ind0] - rayDir[ind0] * rayStart[ind2] / rayDir[ind2];
    } else {
        return false;
    }

    return true;
}

std::shared_ptr<StdMaterial> GizmoPlane::Create(DevicePtr& device, std::shared_ptr<Material>& material, math::Axis2 axises) {
    m_axises = axises;

    auto axisNum0 = static_cast<uint>(axises[0]);
    auto axisNum1 = static_cast<uint>(axises[1]);

    PlaneShape planeShape(axises, math::GetDirection(math::GetThirdAxis(axises)), {m_size, m_size});
    auto translation = dg::float3(0, 0, 0);
    translation[axisNum0] = m_spacing + m_size * .5f;
    translation[axisNum1] = m_spacing + m_size * .5f;
    planeShape.SetTransform(dg::float4x4::Translation(translation));

    auto node = std::make_shared<StdMaterial>(material, ShapeBuilder(device).Join({&planeShape}, "move plane"));
    auto color = dg::float4(0.f, 0.f, 0.f, 1.f);
    color[axisNum0] = 1.0f;
    color[axisNum1] = 1.0f;
    node->SetBaseColor(color);

    dg::float3 v0 = {0, 0, 0};
    v0[axisNum0] = 1.f;
    dg::float3 v1 = {0, 0, 0};
    v1[axisNum1] = 1.f;
    m_planeParams = math::GetPlaneByPointAndTwoVec(dg::float3(0, 0, 0), v0, v1);

    return node;
}

dg::float4x4 GizmoPlane::GetSelectTransform() const {
    auto scale = dg::float3(1.f, 1.f, 1.f);
    scale[static_cast<uint>(m_axises[0])] = m_selectScale;
    scale[static_cast<uint>(m_axises[1])] = m_selectScale;
    return dg::float4x4::Scale(scale);
}

bool GizmoPlane::StartMove(dg::float3 rayStart, dg::float3 rayDir) {
    return GetProjection(rayStart, rayDir, m_startMoveCoord);
}

bool GizmoPlane::GetMoveOffset(dg::float3 rayStart, dg::float3 rayDir, dg::float3& offset) const {
    dg::float3 value;
    if (!GetProjection(rayStart, rayDir, value)) {
        return false;
    }

    offset = m_startMoveCoord - value;

    return true;
}

bool GizmoPlane::IsSelected(dg::float3 rayStart, dg::float3 rayDir) const {
    dg::float3 coord;
    const constexpr float minCoord = m_spacing + (m_size - m_activeSize) * 0.5f;
    const constexpr float maxCoord = m_spacing + (m_size + m_activeSize) * 0.5f;
    if (math::IntersectionRayAndPlane(rayStart, rayDir, m_planeParams, coord)) {
        float v = coord[static_cast<uint>(m_axises[0])];
        if ((v < minCoord) || (v > maxCoord)) {
            return false;
        }
        v = coord[static_cast<uint>(m_axises[1])];
        if ((v < minCoord) || (v > maxCoord)) {
            return false;
        }

        return true;
    }

    return false;
}

bool GizmoPlane::GetProjection(dg::float3 rayStart, dg::float3 rayDir, dg::float3& value) const {
    if (math::IntersectionRayAndPlane(rayStart, rayDir, m_planeParams, value)) {
        value[static_cast<uint>(math::GetThirdAxis(m_axises))] = 0;
        return true;
    }

    return false;
}
