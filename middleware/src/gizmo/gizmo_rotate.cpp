#include "middleware/gizmo/gizmo_rotate.h"

#include <sys/types.h>
#include <initializer_list>

#include "core/math/types.h"
#include "core/math/matrix.h"
#include "core/math/constants.h"
#include "core/render/transform_graph.h"
#include "platforms/default_window_handler.h"


std::shared_ptr<TransformNode> GizmoRotate::Create(DevicePtr& device, const std::shared_ptr<DefaultWindowEventsHandler>& eventHandler) {
    m_root = std::make_shared<TransformNode>();
    m_root->SetVisible(false);
    m_eventHandler = eventHandler;
    for (const auto axis: {math::Axis::X, math::Axis::Y, math::Axis::Z}) {
        auto axisNum = static_cast<uint>(axis);
        m_toruses[axisNum].Create(axis);
        m_torusNodes[axisNum] = m_toruses[axisNum].GetNode(device);
        m_root->AddChild(m_torusNodes[axisNum]);
    }

    return m_root;
}

void GizmoRotate::Update(const math::Ray& ray) {
    if (m_isMoved && (m_eventHandler->IsKeyDown(Key::MouseLeft) || m_eventHandler->IsKeyReleasedFirstTime(Key::MouseLeft))) {
        auto transform = m_selectedObject->GetBaseTransform();
        double offset;
        if (!m_toruses[m_moveItemIndex].GetMoveOffset(ray, offset)) {
            return;
        }
        auto angles = m_startMoveAngles;
        angles[m_moveItemIndex] -= offset;

        math::SetRotate(transform, dg::ToVector3<float>(angles));
        m_selectedObject->SetTransform(transform);
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

    if (m_isMoved) {
        m_startMoveAngles = dg::ToVector3<double>(math::GetRotate(m_selectedObject->GetBaseTransform()));
    }
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
