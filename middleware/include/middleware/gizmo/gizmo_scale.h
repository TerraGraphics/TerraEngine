#pragma once

#include <memory>
#include <sys/types.h>

#include "core/dg/dg.h"
#include "core/math/basic.h"
#include "core/common/ctor.h"
#include "middleware/gizmo/gizmo_base.h"
#include "middleware/gizmo/gizmo_shape.h"


class Material;
class TransformNode;
class DefaultWindowEventsHandler;
class GizmoScale : public IGizmo, Fixed {
public:
    GizmoScale() = default;
    ~GizmoScale() final = default;

public:
    bool IsSelected() const noexcept final { return m_isSelectedArrow || m_isSelectedPlane; }
    bool IsMoved() const noexcept final { return m_isMoved; }

    void Create(DevicePtr& device, const std::shared_ptr<DefaultWindowEventsHandler>& eventHandler,
        std::shared_ptr<Material>& material, std::shared_ptr<TransformNode>& root);
    void Update(dg::float3 rayStart, dg::float3 rayDir) final;
    void SetEnable(bool value) final;
    void SelectNode(const std::shared_ptr<TransformNode>& node) final;

private:
    void UpdateSelect(dg::float3 rayStart, dg::float3 rayDir);

private:
    bool m_isSelectedArrow = false;
    bool m_isSelectedPlane = false;
    bool m_isMoved = false;
    uint m_moveItemIndex = 0;
    dg::float3 m_startScaleValue;
    std::shared_ptr<TransformNode> m_root = nullptr;
    std::shared_ptr<TransformNode> m_arrowNodes[3] = { nullptr };
    std::shared_ptr<TransformNode> m_planeNodes[3] = { nullptr };
    GizmoArrow m_arrows[3];
    GizmoPlane m_planes[3];
    std::shared_ptr<TransformNode> m_selectedObject = nullptr;
    std::shared_ptr<DefaultWindowEventsHandler> m_eventHandler = nullptr;
};