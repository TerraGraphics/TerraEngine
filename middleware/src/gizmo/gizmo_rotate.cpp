#include "middleware/gizmo/gizmo_rotate.h"

#include <sys/types.h>
#include <initializer_list>

#include "core/scene/transform_graph.h"
#include "platforms/default_window_handler.h"
#include "middleware/std_material/std_material.h" // IWYU pragma: keep


void GizmoRotate::Create(DevicePtr& device, const std::shared_ptr<DefaultWindowEventsHandler>& eventHandler,
    std::shared_ptr<Material>& material, std::shared_ptr<TransformNode>& root) {

    m_root = root;
    m_root->SetVisible(false);
    m_eventHandler = eventHandler;
    for (const auto axis: {math::Axis::X, math::Axis::Y, math::Axis::Z}) {
        auto axisNum = static_cast<uint>(axis);
        m_torusNodes[axisNum] = m_root->NewChild(m_toruses[axisNum].Create(device, material, axis));
    }
}

void GizmoRotate::Update(const math::Ray& ray) {
    if (m_isMoved && (m_eventHandler->IsKeyDown(Key::MouseLeft) || m_eventHandler->IsKeyReleasedFirstTime(Key::MouseLeft))) {
    //     auto transform = m_selectedObject->GetBaseTransform();
    //     dg::float3 offset;

    //     if (m_isSelectedArrow) {
    //         if (!m_arrows[m_moveItemIndex].GetMoveOffset(rayStart, rayDir, offset)) {
    //             return;
    //         }
    //     }

    //     if (m_isSelectedPlane) {
    //         if (!m_planes[m_moveItemIndex].GetMoveOffset(rayStart, rayDir, offset)) {
    //             return;
    //         }
    //     }

    //     transform._41 = m_startMoveCoord[0] - offset[0];
    //     transform._42 = m_startMoveCoord[1] - offset[1];
    //     transform._43 = m_startMoveCoord[2] - offset[2];
    //     m_selectedObject->SetTransform(transform);
        return;
    }

    UpdateSelect(ray);
    m_isMoved = false;
    if (!m_isSelected && !m_eventHandler->IsKeyPressedFirstTime(Key::MouseLeft)) {
        return;
    }

    if (m_isSelected) {
        m_isMoved = m_toruses[m_moveItemIndex].StartMove(ray);
    }

    // if (m_isMoved) {
    //     auto& transform = m_selectedObject->GetBaseTransform();
    //     m_startMoveCoord = dg::float3(transform._41, transform._42, transform._43);
    // }
}

void GizmoRotate::SetEnable(bool value) {
    m_root->SetVisible(value);
    m_isSelected = false;
    m_isMoved = false;
}

void GizmoRotate::SelectNode(const std::shared_ptr<TransformNode>& node) {
    m_selectedObject = node;
}

void GizmoRotate::UpdateSelect(const math::Ray& ray) {
     m_isSelected = false;

    for (uint i=0; i!=3; ++i) {
        if (!m_isSelected && m_toruses[i].IsSelected(ray)) {
            m_moveItemIndex = i;
            m_isSelected = true;
            m_torusNodes[i]->SetTransform(m_toruses[i].GetSelectTransform());
        } else {
            m_torusNodes[i]->SetTransform(dg::One4x4);
        }
    }
}
