#include "middleware/gizmo/gizmo_scale.h"

#include <initializer_list>

#include "core/math/matrix.h"
#include "core/math/constants.h"
#include "core/render/transform_graph.h"
#include "platforms/default_window_handler.h"


std::shared_ptr<TransformNode> GizmoScale::Create(DevicePtr& device, const std::shared_ptr<DefaultWindowEventsHandler>& eventHandler) {
    m_root = std::make_shared<TransformNode>();
    m_root->SetVisible(false);
    m_eventHandler = eventHandler;
    for (const auto axis: {math::Axis::X, math::Axis::Y, math::Axis::Z}) {
        auto axisNum = static_cast<uint>(axis);
        m_arrows[axisNum].Create(axis);
        m_arrowNodes[axisNum] = m_arrows[axisNum].GetNode(device, false);
        m_root->AddChild(m_arrowNodes[axisNum]);

        m_planes[axisNum].Create({axis, math::Next(axis)});
        m_planeNodes[axisNum] = m_planes[axisNum].GetNode(device);
        m_root->AddChild(m_planeNodes[axisNum]);
    }

    return m_root;
}

void GizmoScale::Update(const math::Ray& ray) {
    if (m_isMoved && (m_eventHandler->IsKeyDown(Key::MouseLeft) || m_eventHandler->IsKeyReleasedFirstTime(Key::MouseLeft))) {
        auto transform = m_selectedObject->GetBaseTransform();
        dg::float3 offset;

        if (m_isSelectedArrow) {
            if (!m_arrows[m_moveItemIndex].GetMoveOffset(ray, offset)) {
                return;
            }
        }

        if (m_isSelectedPlane) {
            if (!m_planes[m_moveItemIndex].GetMoveOffset(ray, offset)) {
                return;
            }
        }

        math::SetScale(transform, m_startScaleValue - offset);
        m_selectedObject->SetTransform(transform);
        return;
    }

    UpdateSelect(ray);
    m_isMoved = false;
    if (!m_isSelectedArrow && !m_isSelectedPlane && !m_eventHandler->IsKeyPressedFirstTime(Key::MouseLeft)) {
        return;
    }

    if (m_isSelectedArrow) {
        m_isMoved = m_arrows[m_moveItemIndex].StartMove(ray);
    }
    if (m_isSelectedPlane) {
        m_isMoved = m_planes[m_moveItemIndex].StartMove(ray);
    }

    if (m_isMoved) {
        m_startScaleValue = math::GetScale(m_selectedObject->GetBaseTransform());
    }
}

void GizmoScale::SetEnable(bool value) {
    m_root->SetVisible(value);
    m_isMoved = false;
    m_isSelectedArrow = false;
    m_isSelectedPlane = false;
}

void GizmoScale::SelectNode(const std::shared_ptr<TransformNode>& node) {
    m_selectedObject = node;
}

void GizmoScale::UpdateSelect(const math::Ray& ray) {
    m_isSelectedArrow = false;
    m_isSelectedPlane = false;

    for (uint i=0; i!=3; ++i) {
        if (!m_isSelectedArrow && m_arrows[i].IsSelected(ray)) {
            m_moveItemIndex = i;
            m_isSelectedArrow = true;
            m_arrowNodes[i]->SetTransform(m_arrows[i].GetSelectTransform());
        } else {
            m_arrowNodes[i]->SetTransform(dg::One4x4);
        }
    }
    for (uint i=0; i!=3; ++i) {
        if (!m_isSelectedArrow && !m_isSelectedPlane && m_planes[i].IsSelected(ray)) {
            m_moveItemIndex = i;
            m_isSelectedPlane = true;
            m_planeNodes[i]->SetTransform(m_planes[i].GetSelectTransform());
        } else {
            m_planeNodes[i]->SetTransform(dg::One4x4);
        }
    }
}
