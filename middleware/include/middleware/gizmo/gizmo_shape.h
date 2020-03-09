#pragma once

#include <memory>

#include "core/dg/dg.h"
#include "core/math/basic.h"
#include "core/common/ctor.h"


class Material;
class StdMaterial;
class GizmoArrow : Fixed {
public:
    GizmoArrow() = default;
    ~GizmoArrow() = default;

    std::shared_ptr<StdMaterial> Create(DevicePtr& device, std::shared_ptr<Material>& material, math::Axis axis, bool isMoveType);
    bool StartMove(dg::float3 rayStart, dg::float3 rayDir);
    bool GetMoveOffset(dg::float3 rayStart, dg::float3 rayDir, dg::float3& offset) const;

    dg::float4x4 GetSelectTransform() const;
    bool IsSelected(dg::float3 rayStart, dg::float3 rayDir) const;

private:
    bool GetProjection(dg::float3 rayStart, dg::float3 rayDir, float& value) const;

private:
    static constexpr float m_height = 1.f;
    static constexpr float m_radius = .01f;
    static constexpr float m_activeRadius = .04f;
    static constexpr float m_selectScale = 1.5f;

    math::Axis m_axis;
    float m_startMoveCoord;
};

class GizmoPlane : Fixed {
public:
    GizmoPlane() = default;
    ~GizmoPlane() = default;

    std::shared_ptr<StdMaterial> Create(DevicePtr& device, std::shared_ptr<Material>& material, math::Axis2 axises);
    bool StartMove(dg::float3 rayStart, dg::float3 rayDir);
    bool GetMoveOffset(dg::float3 rayStart, dg::float3 rayDir, dg::float3& offset) const;

    dg::float4x4 GetSelectTransform() const;
    bool IsSelected(dg::float3 rayStart, dg::float3 rayDir) const;

private:
    bool GetProjection(dg::float3 rayStart, dg::float3 rayDir, dg::float3& value) const;

private:
    static constexpr float m_spacing = .1f;
    static constexpr float m_size = .2f;
    static constexpr float m_activeSize = .3f;
    static constexpr float m_selectScale = 1.1f;

    math::Axis2 m_axises = {math::Axis::X, math::Axis::Y};
    dg::float4 m_planeParams;
    dg::float3 m_startMoveCoord;
};
