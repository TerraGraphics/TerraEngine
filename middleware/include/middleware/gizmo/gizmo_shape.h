#pragma once

#include <memory>

#include "core/dg/dg.h"
#include "core/math/types.h"
#include "core/common/ctor.h"


class Material;
class StdMaterial;
class GizmoArrow : Fixed {
public:
    GizmoArrow() = default;
    ~GizmoArrow() = default;

    std::shared_ptr<StdMaterial> Create(DevicePtr& device, std::shared_ptr<Material>& material, math::Axis axis, bool isMoveType);
    bool StartMove(const math::Ray& ray);
    bool GetMoveOffset(const math::Ray& ray, dg::float3& offset) const;

    dg::float4x4 GetSelectTransform() const;
    bool IsSelected(const math::Ray& ray) const;

private:
    bool GetProjection(const math::Ray& ray, float& value) const;

private:
    static constexpr float m_height = 1.f;
    static constexpr float m_radius = .01f;
    static constexpr float m_activeRadius = .04f;
    static constexpr float m_selectScale = 1.5f;

    math::Axis m_axis;
    float m_startMoveCoord;
    math::Cylinder m_activeCylinder;
};

class GizmoPlane : Fixed {
public:
    GizmoPlane() = default;
    ~GizmoPlane() = default;

    std::shared_ptr<StdMaterial> Create(DevicePtr& device, std::shared_ptr<Material>& material, math::Axis2 axises);
    bool StartMove(const math::Ray& ray);
    bool GetMoveOffset(const math::Ray& ray, dg::float3& offset) const;

    dg::float4x4 GetSelectTransform() const;
    bool IsSelected(const math::Ray& ray) const;

private:
    bool GetProjection(const math::Ray& ray, dg::double3& value) const;

private:
    static constexpr float m_spacing = .1f;
    static constexpr float m_size = .2f;
    static constexpr float m_activeSize = .3f;
    static constexpr float m_selectScale = 1.1f;

    math::Axis2 m_axises = {math::Axis::X, math::Axis::Y};
    math::Plane m_planeParams;
    dg::double3 m_startMoveCoord;
};

class GizmoTorus : Fixed {
public:
    GizmoTorus() = default;
    ~GizmoTorus() = default;

    std::shared_ptr<StdMaterial> Create(DevicePtr& device, std::shared_ptr<Material>& material, math::Axis axis);
    bool StartMove(const math::Ray& ray);
    bool GetMoveOffset(const math::Ray& ray, dg::float3& offset) const;

    dg::float4x4 GetSelectTransform() const;
    bool IsSelected(const math::Ray& ray) const;

private:
    bool GetProjection(const math::Ray& ray, dg::float3& value) const;

private:
    static constexpr float m_majorRadis = 0.5f;
    static constexpr float m_minorRadis = 0.02f;

    math::Axis m_axis;
};
