#include "middleware/gizmo/gizmo_3d.h"

#include <utility>

#include "core/engine.h"
#include "core/camera/camera.h"
#include "core/render/transform_graph.h"
#include "middleware/gizmo/gizmo_base.h"
#include "middleware/gizmo/gizmo_move.h"
#include "middleware/gizmo/gizmo_scale.h"
#include "middleware/gizmo/gizmo_rotate.h"
#include "platforms/default_window_handler.h"


Gizmo3D::Gizmo3D() {

}

Gizmo3D::~Gizmo3D() {
    m_rootNode.reset();
    m_selectedObject.reset();
    m_eventHandler.reset();
}

std::shared_ptr<TransformNode> Gizmo3D::Create() {
    auto& engine = Engine::Get();
    auto& device = engine.GetDevice();
    m_eventHandler = engine.GetEventHandler();

    m_rootNode = std::make_shared<TransformNode>();

    auto move = std::make_unique<GizmoMove>();
    m_rootNode->AddChild(move->Create(device, m_eventHandler));
    m_gizmos[static_cast<uint32_t>(Type::MOVE)] = std::move(move);

    auto rotate = std::make_unique<GizmoRotate>();
    m_rootNode->AddChild(rotate->Create(device, m_eventHandler));
    m_gizmos[static_cast<uint32_t>(Type::ROTATE)] = std::move(rotate);

    auto scale = std::make_unique<GizmoScale>();
    m_rootNode->AddChild(scale->Create(device, m_eventHandler));
    m_gizmos[static_cast<uint32_t>(Type::SCALE)] = std::move(scale);

    SetType(m_type);

    return m_rootNode;
}

void Gizmo3D::Update(const std::shared_ptr<Camera>& camera, math::RectF windowRect, bool mouseUnderWindow, GizmoFoundDesc& foundDesc) {
    if (!mouseUnderWindow) {
        return;
    }

    bool mouseFirstRelease = m_eventHandler->IsKeyReleasedFirstTime(Key::MouseLeft);

    float absoluteMousePosX, absoluteMousePosY;
    m_eventHandler->GetCursorPosition(absoluteMousePosX, absoluteMousePosY);
    auto mousePos = math::PointF(absoluteMousePosX - windowRect.x, absoluteMousePosY - windowRect.y);

    foundDesc.needFound = false;
    foundDesc.mouseX = static_cast<uint32_t>(mousePos.x);
    foundDesc.mouseY = static_cast<uint32_t>(mousePos.y);

    if (!m_selectedObject) {
        if (mouseFirstRelease) {
            foundDesc.needFound = true;
        }
        return;
    }

    auto nodeMatrix = m_selectedObject->GetWorldMatrix();
    auto nodePos = dg::float3(nodeMatrix._41, nodeMatrix._42, nodeMatrix._43);
    auto nodeToCamDir = dg::normalize(nodePos - camera->GetPosition());
    auto gizmoPos = camera->GetPosition() + nodeToCamDir * 6.f;

    nodeMatrix._11 = 1.f;
    nodeMatrix._22 = 1.f;
    nodeMatrix._33 = 1.f;
    nodeMatrix._41 = gizmoPos.x;
    nodeMatrix._42 = gizmoPos.y;
    nodeMatrix._43 = gizmoPos.z;
    nodeMatrix._44 = 1.f;
    m_rootNode->SetTransform(nodeMatrix);

    if (!m_activeGizmo->IsMoved()) {
        m_invRayMatrix = dg::ToMatrix4x4<double>(nodeMatrix).Inverse();
    }

    auto ray = math::Ray(dg::ToVector3<double>(camera->GetPosition()), camera->ScreenPointToRay(mousePos, windowRect.Size()));
    ray *= m_invRayMatrix;

    m_activeGizmo->Update(ray);
    if (mouseFirstRelease && !m_activeGizmo->IsSelected() && !m_activeGizmo->IsMoved()) {
        foundDesc.needFound = true;
    }
}

void Gizmo3D::SelectNode(const std::shared_ptr<TransformNode>& node) {
    m_selectedObject = node;
    m_rootNode->SetVisible(static_cast<bool>(node));
    m_activeGizmo->SelectNode(node);
}

void Gizmo3D::SetType(Type value) {
    if ((m_type != Type::MOVE) && (m_type != Type::ROTATE) && (m_type != Type::SCALE)) {
        return;
    }
    if ((m_type == value) && (m_activeGizmo != nullptr)) {
        return;
    }

    m_type = value;

    if (m_activeGizmo != nullptr) {
        m_activeGizmo->SetEnable(false);
    }
    m_activeGizmo = m_gizmos[static_cast<uint32_t>(value)].get();
    m_activeGizmo->SetEnable(true);
    SelectNode(m_selectedObject);
}
