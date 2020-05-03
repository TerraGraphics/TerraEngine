#include "middleware/gizmo/gizmo_shape.h"

#include <cmath>
#include <sys/types.h>

#include "core/math/intersection.h"
#include "core/scene/transform_graph.h"
#include "middleware/generator/mesh_generator.h"
#include "middleware/std_material/std_material.h"


class Geometry;

void GizmoArrow::Create(math::Axis axis) {
    m_axis = axis;
    m_activeCylinder = math::Cylinder(m_activeRadius, m_height, m_axis);
}

std::shared_ptr<TransformNode> GizmoArrow::GetNode(DevicePtr& device, std::shared_ptr<Material>& material, bool isMoveType) {
    auto axisNum = static_cast<uint>(m_axis);
    float arrowSpacing = m_height * .05f;
    auto center = dg::float3(0, 0, 0);
    auto color = dg::float4(0.f, 0.f, 0.f, 1.f);

    float cubeSide = m_height * .08f;
    float coneHeight = m_height * .1f;

    float cylinderHeight = m_height - arrowSpacing - (isMoveType ? coneHeight : cubeSide);
    CylinderShape cylinderShape({5, 1}, m_axis, m_radius, cylinderHeight);
    center[axisNum] = arrowSpacing + cylinderHeight * 0.5f;
    cylinderShape.SetCenter(center);

    std::shared_ptr<Geometry> geometry;
    if (isMoveType) {
        float coneRadius = coneHeight * .3f;
        ConeShape coneShape({10, 1}, m_axis, coneRadius, coneHeight);
        center[axisNum] = m_height - coneHeight * 0.5f;
        coneShape.SetCenter(center);
        geometry = ShapeBuilder(device).Join({&coneShape, &cylinderShape}, "gizmo arrow");
    } else {
        CubeShape cubeShape({cubeSide, cubeSide, cubeSide});
        center[axisNum] = m_height - cubeSide * 0.5f;
        cubeShape.SetCenter(center);
        geometry = ShapeBuilder(device).Join({&cubeShape, &cylinderShape}, "gizmo arrow");
    }

    color[axisNum] = 1.0f;
    auto materialInstance = std::make_shared<StdMaterial>(material);
    materialInstance->SetBaseColor(color);

    return std::make_shared<TransformNode>(geometry, materialInstance);
}

bool GizmoArrow::StartMove(const math::Ray& ray) {
    return GetProjection(ray, m_startMoveCoord);
}

bool GizmoArrow::GetMoveOffset(const math::Ray& ray, dg::float3& offset) const {
    float value;
    if (!GetProjection(ray, value)) {
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

bool GizmoArrow::IsSelected(const math::Ray& ray) const {
    return math::IsIntersection(ray, m_activeCylinder);
}

/*
    for axis == X: ray (start; dir) crosses a plane X0Y (z == 0) or X0Z (y == 0)

    x = x0 + l*t
    y = y0 + m*t
    z = z0 + n*t

    for X0Y: x = x0 + l*t, where t = -z0 / n
    For X0Z: x = x0 + l*t, where t = -y0 / m
*/
bool GizmoArrow::GetProjection(const math::Ray& ray, float& value) const {
    auto ind0 = static_cast<uint>(m_axis);
    auto ind1 = static_cast<uint>(math::Next(m_axis));
    auto ind2 = static_cast<uint>(math::Prev(m_axis));

    if (std::fpclassify(ray.direction[ind1]) != FP_ZERO) {
        value = static_cast<float>(ray.start[ind0] - ray.direction[ind0] * ray.start[ind1] / ray.direction[ind1]);
    } else if (std::fpclassify(ray.direction[ind2]) != FP_ZERO) {
        value = static_cast<float>(ray.start[ind0] - ray.direction[ind0] * ray.start[ind2] / ray.direction[ind2]);
    } else {
        return false;
    }

    return true;
}

void GizmoPlane::Create(math::Axis2 axises) {
    m_axises = axises;

    dg::double3 v0 = {0, 0, 0};
    v0[static_cast<uint>(axises[0])] = 1;
    dg::double3 v1 = {0, 0, 0};
    v1[static_cast<uint>(axises[1])] = 1;
    m_planeParams = math::Plane(dg::double3(0, 0, 0), v0, v1);
}

std::shared_ptr<TransformNode> GizmoPlane::GetNode(DevicePtr& device, std::shared_ptr<Material>& material) {
    auto axisNum0 = static_cast<uint>(m_axises[0]);
    auto axisNum1 = static_cast<uint>(m_axises[1]);

    PlaneShape planeShape(m_axises, math::GetDirection(math::GetThirdAxis(m_axises)), {m_size, m_size});
    auto center = dg::float3(0, 0, 0);
    center[axisNum0] = m_spacing + m_size * .5f;
    center[axisNum1] = m_spacing + m_size * .5f;
    planeShape.SetCenter(center);
    auto geometry = ShapeBuilder(device).Join({&planeShape}, "gizmo plane");

    auto materialInstance = std::make_shared<StdMaterial>(material);
    auto color = dg::float4(0.f, 0.f, 0.f, 1.f);
    color[axisNum0] = 1.0f;
    color[axisNum1] = 1.0f;
    materialInstance->SetBaseColor(color);

    return std::make_shared<TransformNode>(geometry, materialInstance);
}

dg::float4x4 GizmoPlane::GetSelectTransform() const {
    auto scale = dg::float3(1.f, 1.f, 1.f);
    scale[static_cast<uint>(m_axises[0])] = m_selectScale;
    scale[static_cast<uint>(m_axises[1])] = m_selectScale;
    return dg::float4x4::Scale(scale);
}

bool GizmoPlane::StartMove(const math::Ray& ray) {
    return GetProjection(ray, m_startMoveCoord);
}

bool GizmoPlane::GetMoveOffset(const math::Ray& ray, dg::float3& offset) const {
    dg::double3 value;
    if (!GetProjection(ray, value)) {
        return false;
    }

    offset = dg::ToVector3<float>(m_startMoveCoord - value);

    return true;
}

bool GizmoPlane::IsSelected(const math::Ray& ray) const {
    dg::double3 coord;
    const constexpr double minCoord = static_cast<double>(m_spacing + (m_size - m_activeSize) * 0.5f);
    const constexpr double maxCoord = minCoord + static_cast<double>(m_activeSize);
    if (math::Intersection(ray, m_planeParams, coord) != 0) {
        auto v = coord[static_cast<uint>(m_axises[0])];
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

bool GizmoPlane::GetProjection(const math::Ray& ray, dg::double3& value) const {
    if (math::Intersection(ray, m_planeParams, value) != 0) {
        value[static_cast<uint>(math::GetThirdAxis(m_axises))] = 0;
        return true;
    }

    return false;
}

void GizmoTorus::Create(math::Axis axis) {
    m_axis = axis;
    m_activeTorus = math::Torus(m_activeRadis, m_majorRadis, axis);
}

std::shared_ptr<TransformNode> GizmoTorus::GetNode(DevicePtr& device, std::shared_ptr<Material>& material) {
    TorusShape torusShape(m_minorRadis, m_majorRadis, 10, 30, m_axis);
    auto geometry = ShapeBuilder(device).Join({&torusShape}, "gizmo torus");

    auto materialInstance = std::make_shared<StdMaterial>(material);
    auto color = dg::float4(0.f, 0.f, 0.f, 1.f);
    color[static_cast<uint>(m_axis)] = 1.0f;
    materialInstance->SetBaseColor(color);

    return std::make_shared<TransformNode>(geometry, materialInstance);
}

bool GizmoTorus::StartMove(const math::Ray& ray) {
    return GetProjection(ray, m_startPolarAngle);
}

bool GizmoTorus::GetMoveOffset(const math::Ray& ray, double& offset) const {
    double value;
    if (!GetProjection(ray, value)) {
        return false;
    }

    offset = m_startPolarAngle - value;

    return true;
}

dg::float4x4 GizmoTorus::GetSelectTransform() const {
    auto scale = dg::float3(m_selectScale, m_selectScale, m_selectScale);
    return dg::float4x4::Scale(scale);
}

bool GizmoTorus::IsSelected(const math::Ray& ray) const {
    return math::IsIntersection(ray, m_activeTorus);
}

/*
    for axis == Z: ray crosses a plane X0Y (z == 0)

    ray:
        z = z0 + n*t = 0 => t = -z0 / n
        x = x0 + l*t = x0 + l*(-z0 / n)
        y = y0 + m*t = y0 + m*(-z0 / n)

    Find and return angle in polar coordinate system in [-pi; +pi]
*/
bool GizmoTorus::GetProjection(const math::Ray& ray, double& value) const {
    auto ind0 = static_cast<uint>(m_axis);
    auto indX2d = static_cast<uint>(math::Next(m_axis));
    auto indY2d = static_cast<uint>(math::Prev(m_axis));

    if (std::fpclassify(ray.direction[ind0]) == FP_ZERO) {
        return false;
    }

    auto t = -ray.start[ind0] / ray.direction[ind0];
    auto intersectionPoint = ray[t];

    auto pointInPlane = dg::normalize(dg::double2(intersectionPoint[indX2d], intersectionPoint[indY2d]));
    if ((std::fpclassify(pointInPlane.x) == FP_ZERO) && (std::fpclassify(pointInPlane.y) == FP_ZERO)) {
        return false;
    }
    value = std::atan2(pointInPlane.x, pointInPlane.y);

    return true;
}
