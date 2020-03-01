#pragma once

#include <memory>
#include <sys/types.h>

#include "core/dg/dg.h"
#include "core/math/basic.h"
#include "core/common/ctor.h"
#include "middleware/gizmo/gizmo_shape.h"


class Material;
class TransformNode;
class DefaultWindowEventsHandler;
class GizmoMove : Fixed {
public:
    GizmoMove() = default;
    ~GizmoMove() = default;

public:
    void Create(DevicePtr& device, const std::shared_ptr<DefaultWindowEventsHandler>& eventHandler,
        std::shared_ptr<Material>& material, std::shared_ptr<TransformNode>& root);
    void Update(dg::float3 rayStart, dg::float3 rayDir);
    void SelectNode(const std::shared_ptr<TransformNode>& node);

    bool IsSelected() const noexcept { return m_isSelectedArrow || m_isSelectedPlane; }
    bool IsMoved() const noexcept { return m_isMoved; }

private:
    void UpdateSelect(dg::float3 rayStart, dg::float3 rayDir);

private:
    bool m_isSelectedArrow = false;
    bool m_isSelectedPlane = false;
    bool m_isMoved = false;
    uint m_moveItemIndex = 0;
    dg::float3 m_startMoveCoord;
    std::shared_ptr<TransformNode> m_root = nullptr;
    std::shared_ptr<TransformNode> m_arrowNodes[3] = { nullptr };
    std::shared_ptr<TransformNode> m_planeNodes[3] = { nullptr };
    GizmoArrow m_arrows[3];
    GizmoPlane m_planes[3];
    std::shared_ptr<TransformNode> m_selectedObject = nullptr;
    std::shared_ptr<DefaultWindowEventsHandler> m_eventHandler = nullptr;
};
