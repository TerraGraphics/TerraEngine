#include "middleware/gizmo/gizmo_move.h"

#include "core/math/advanced.h"
#include "core/scene/transform_graph.h"
#include "middleware/generator/generator.h"
#include "platforms/default_window_handler.h"
#include "middleware/std_material/std_material.h"


void GizmoMove::Create(DevicePtr& device, const std::shared_ptr<DefaultWindowEventsHandler>& eventHandler,
    std::shared_ptr<Material>& material, std::shared_ptr<TransformNode>& root) {

    m_eventHandler = eventHandler;
    m_root = root;
    for (const auto axis: {math::Axis::X, math::Axis::Y, math::Axis::Z}) {
        auto translation = dg::float3(0, 0, 0);

        float coneHeight = 0.1f;
        ConeShape coneShape({10, 1}, axis, 0.03f, coneHeight);
        translation[static_cast<uint>(axis)] = 1.f - coneHeight * 0.5f;
        coneShape.SetTranform(dg::float4x4::Translation(translation));

        float cylinderSpacing = 0.05f;
        float cylinderHeight = 1.f - coneHeight - cylinderSpacing;
        CylinderShape cylinderShape({5, 1}, axis, m_arrowRadius, cylinderHeight);
        translation[static_cast<uint>(axis)] = cylinderSpacing + cylinderHeight * 0.5f;
        cylinderShape.SetTranform(dg::float4x4::Translation(translation));

        auto arrowNode = std::make_shared<StdMaterial>(material, ShapeBuilder(device).Join({&coneShape, &cylinderShape}, "arrow"));
        auto color = dg::float4(0.f, 0.f, 0.f, 1.f);
        color[static_cast<uint>(axis)] = 1.0f;
        arrowNode->SetBaseColor(color);

        m_arrowNodes[static_cast<uint>(axis)] = m_root->NewChild(arrowNode);
    }
}

void GizmoMove::Update(dg::float3 rayStart, dg::float3 rayDir) {
    if ((m_isMoved) && (m_eventHandler->IsKeyDown(Key::MouseLeft) || m_eventHandler->IsKeyReleasedFirstTime(Key::MouseLeft))) {
        float coord;
        if (FindCoordByAxis(rayStart, rayDir, m_moveAxis, coord)) {
            auto transform = m_selectedObject->GetBaseTransform();
            auto ind = static_cast<uint>(m_moveAxis);
            transform[3][ind] = coord - m_startMoveAxisCoord + m_startMoveSelectedCoord[ind];
            m_selectedObject->SetTransform(transform);
        }

        return;
    }

    SelectReset();
    m_isMoved = false;
    m_isSelected = FindSelect(rayStart, rayDir, m_moveAxis);
    if (m_isSelected) {
        SelectAxis(m_moveAxis);
        if (m_eventHandler->IsKeyPressedFirstTime(Key::MouseLeft)) {
            m_isMoved = FindCoordByAxis(rayStart, rayDir, m_moveAxis, m_startMoveAxisCoord);
            if (m_isMoved) {
                auto& transform = m_selectedObject->GetBaseTransform();
                m_startMoveSelectedCoord = dg::float3(transform._41, transform._42, transform._43);
            }
        }
    }
}

void GizmoMove::SelectNode(const std::shared_ptr<TransformNode>& node) {
    m_selectedObject = node;
}

void GizmoMove::SelectReset() {
    m_arrowNodes[static_cast<uint>(math::Axis::X)]->SetTransform(dg::One4x4);
    m_arrowNodes[static_cast<uint>(math::Axis::Y)]->SetTransform(dg::One4x4);
    m_arrowNodes[static_cast<uint>(math::Axis::Z)]->SetTransform(dg::One4x4);
}

void GizmoMove::SelectAxis(math::Axis value) {
    switch (value) {
    case math::Axis::X:
        m_arrowNodes[static_cast<uint>(math::Axis::X)]->SetTransform(dg::float4x4::Scale(1.f, m_arrowSelectScale, m_arrowSelectScale));
        break;
    case math::Axis::Y:
        m_arrowNodes[static_cast<uint>(math::Axis::Y)]->SetTransform(dg::float4x4::Scale(m_arrowSelectScale, 1.f, m_arrowSelectScale));
        break;
    case math::Axis::Z:
        m_arrowNodes[static_cast<uint>(math::Axis::Z)]->SetTransform(dg::float4x4::Scale(m_arrowSelectScale, m_arrowSelectScale, 1.f));
        break;
    default:
        break;
    }
}

bool GizmoMove::FindSelect(dg::float3 rayStart, dg::float3 rayDir, math::Axis& result) {
    if (math::IntersectionRayAndCylinder(rayStart, rayDir, math::Axis::X, m_arrowActiveRadius, 1.f)) {
        result = math::Axis::X;
    } else if (math::IntersectionRayAndCylinder(rayStart, rayDir, math::Axis::Y, m_arrowActiveRadius, 1.f)) {
        result = math::Axis::Y;
    } else if (math::IntersectionRayAndCylinder(rayStart, rayDir, math::Axis::Z, m_arrowActiveRadius, 1.f)) {
        result = math::Axis::Z;
    } else {
        return false;
    }

    return true;
}

/*
    for axis == X: ray (rayStart; rayDir) crosses a plane X0Y (z == 0) or X0Z (y == 0)

    x = x0 + l*t
    y = y0 + m*t
    z = z0 + n*t

    for X0Y: x = x0 + l*t, where t = -z0 / n
    For X0Z: x = x0 + l*t, where t = -y0 / m
*/
bool GizmoMove::FindCoordByAxis(dg::float3 rayStart, dg::float3 rayDir, math::Axis axis, float& result) {
    auto ind0 = static_cast<uint>(axis);
    auto axis1 = math::Next(axis);
    auto ind1 = static_cast<uint>(axis1);
    auto axis2 = math::Next(axis1);
    auto ind2 = static_cast<uint>(axis2);

    if (std::fpclassify(rayDir[ind1]) != FP_ZERO) {
        result = rayStart[ind0] - rayDir[ind0] * rayStart[ind1] / rayDir[ind1];
    } else if (std::fpclassify(rayDir[ind2]) != FP_ZERO) {
        result = rayStart[ind0] - rayDir[ind0] * rayStart[ind2] / rayDir[ind2];
    } else {
        return false;
    }

    return true;
}
