#include "middleware/gizmo/gizmo_scale.h"

#include <initializer_list>

#include "core/math/constants.h"
#include "core/scene/transform_graph.h"
#include "platforms/default_window_handler.h"
#include "middleware/std_material/std_material.h" // IWYU pragma: keep


void GizmoScale::Create(DevicePtr& device, const std::shared_ptr<DefaultWindowEventsHandler>& eventHandler,
    std::shared_ptr<Material>& material, std::shared_ptr<TransformNode>& root) {

    m_root = root;
    m_root->SetVisible(false);
    m_eventHandler = eventHandler;
    for (const auto axis: {math::Axis::X, math::Axis::Y, math::Axis::Z}) {
        auto axisNum = static_cast<uint>(axis);
        m_arrowNodes[axisNum] = m_root->NewChild(m_arrows[axisNum].Create(device, material, axis, false));
        m_planeNodes[axisNum] = m_root->NewChild(m_planes[axisNum].Create(device, material, {axis, math::Next(axis)}));
    }
}

void GizmoScale::Update(dg::float3 rayStart, dg::float3 rayDir) {
    if (m_isMoved && (m_eventHandler->IsKeyDown(Key::MouseLeft) || m_eventHandler->IsKeyReleasedFirstTime(Key::MouseLeft))) {
        auto transform = m_selectedObject->GetBaseTransform();
        dg::float3 offset;

        if (m_isSelectedArrow) {
            if (!m_arrows[m_moveItemIndex].GetMoveOffset(rayStart, rayDir, offset)) {
                return;
            }
        }

        if (m_isSelectedPlane) {
            if (!m_planes[m_moveItemIndex].GetMoveOffset(rayStart, rayDir, offset)) {
                return;
            }
        }

        transform._11 = m_startScaleValue[0] - offset[0];
        transform._22 = m_startScaleValue[1] - offset[1];
        transform._33 = m_startScaleValue[2] - offset[2];
        m_selectedObject->SetTransform(transform);
        return;
    }

    UpdateSelect(rayStart, rayDir);
    m_isMoved = false;
    if (!m_isSelectedArrow && !m_isSelectedPlane && !m_eventHandler->IsKeyPressedFirstTime(Key::MouseLeft)) {
        return;
    }

    if (m_isSelectedArrow) {
        m_isMoved = m_arrows[m_moveItemIndex].StartMove(rayStart, rayDir);
    }
    if (m_isSelectedPlane) {
        m_isMoved = m_planes[m_moveItemIndex].StartMove(rayStart, rayDir);
    }

    if (m_isMoved) {
        auto& transform = m_selectedObject->GetBaseTransform();
        m_startScaleValue = dg::float3(transform._11, transform._22, transform._33);
    }
}

void GizmoScale::SetEnable(bool value) {
    m_root->SetVisible(value);
}

void GizmoScale::SelectNode(const std::shared_ptr<TransformNode>& node) {
    m_selectedObject = node;
}

void GizmoScale::UpdateSelect(dg::float3 rayStart, dg::float3 rayDir) {
    m_isSelectedArrow = false;
    m_isSelectedPlane = false;

    for (uint i=0; i!=3; ++i) {
        if (!m_isSelectedArrow && m_arrows[i].IsSelected(rayStart, rayDir)) {
            m_moveItemIndex = i;
            m_isSelectedArrow = true;
            m_arrowNodes[i]->SetTransform(m_arrows[i].GetSelectTransform());
        } else {
            m_arrowNodes[i]->SetTransform(dg::One4x4);
        }
    }
    for (uint i=0; i!=3; ++i) {
        if (!m_isSelectedArrow && !m_isSelectedPlane && m_planes[i].IsSelected(rayStart, rayDir)) {
            m_moveItemIndex = i;
            m_isSelectedPlane = true;
            m_planeNodes[i]->SetTransform(m_planes[i].GetSelectTransform());
        } else {
            m_planeNodes[i]->SetTransform(dg::One4x4);
        }
    }
}