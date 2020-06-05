#pragma once

#include <memory>

#include "dg/dg.h"
#include "core/math/types.h"
#include "core/common/ctor.h"


class TransformNode;
class GizmoArrow : Fixed {
public:
    GizmoArrow() = default;
    ~GizmoArrow() = default;

    void Create(math::Axis axis);

    std::shared_ptr<TransformNode> GetNode(DevicePtr& device, bool isMoveType);

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

    void Create(math::Axis2 axises);

    std::shared_ptr<TransformNode> GetNode(DevicePtr& device);

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

    void Create(math::Axis axis);

    std::shared_ptr<TransformNode> GetNode(DevicePtr& device);

    bool StartMove(const math::Ray& ray);
    bool GetMoveOffset(const math::Ray& ray, double& offset) const;

    dg::float4x4 GetSelectTransform() const;
    bool IsSelected(const math::Ray& ray) const;

private:
    bool GetProjection(const math::Ray& ray, double& value) const;

private:
    static constexpr float m_majorRadis = 0.5f;
    static constexpr float m_minorRadis = 0.02f;
    static constexpr float m_activeRadis = 0.05f;
    static constexpr float m_selectScale = 1.1f;

    math::Axis m_axis;
    math::Torus m_activeTorus;
    double m_startPolarAngle;
};
