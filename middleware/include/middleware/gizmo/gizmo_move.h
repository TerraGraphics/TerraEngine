#pragma once

#include "core/dg/dg.h"
#include "core/math/basic.h"
#include "core/common/ctor.h"


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

    bool IsSelected() const noexcept { return m_isSelected; }
    bool IsMoved() const noexcept { return m_isMoved; }

private:
    void SelectReset();
    void SelectAxis(math::Axis value);
    bool FindSelect(dg::float3 rayStart, dg::float3 rayDir, math::Axis& result);
    bool FindCoordByAxis(dg::float3 rayStart, dg::float3 rayDir, math::Axis axis, float& result);

private:
    bool m_isSelected = false;
    bool m_isMoved = false;
    math::Axis m_moveAxis;
    float m_startMoveAxisCoord;
    dg::float3 m_startMoveSelectedCoord;
    std::shared_ptr<TransformNode> m_root = nullptr;
    std::shared_ptr<TransformNode> m_arrowNodes[3] = { nullptr };
    std::shared_ptr<TransformNode> m_selectedObject = nullptr;
    std::shared_ptr<DefaultWindowEventsHandler> m_eventHandler = nullptr;

    static constexpr float m_arrowRadius = 0.01f;
    static constexpr float m_arrowActiveRadius = 0.04f;
    static constexpr float m_arrowSelectScale = 1.5f;
};
