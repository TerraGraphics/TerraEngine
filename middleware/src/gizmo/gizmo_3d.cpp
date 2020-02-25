#include "middleware/gizmo/gizmo_3d.h"

#include "core/camera/camera.h"
#include "core/scene/vertexes.h"
#include "core/scene/transform_graph.h"
#include "middleware/gizmo/gizmo_move.h"
#include "core/material/material_builder.h"
#include "platforms/default_window_handler.h"


Gizmo3D::Gizmo3D()
    : m_move(new GizmoMove()) {

}

Gizmo3D::~Gizmo3D() {
    m_move.reset();
    m_rootNode.reset();
    m_selectedObject.reset();
    m_eventHandler.reset();
}

std::shared_ptr<TransformNode> Gizmo3D::Create(DevicePtr& device, const std::shared_ptr<DefaultWindowEventsHandler>& eventHandler,
    std::shared_ptr<MaterialBuilder>& materialBuilder, const VertexDecl& additionalVertexDecl) {

    auto material = materialBuilder->Create(materialBuilder->GetShaderMask("BASE_COLOR_MATERIAL"), VertexPNC::GetDecl(), additionalVertexDecl).
        DepthEnable(false).
        CullMode(dg::CULL_MODE_NONE).
        Var(dg::SHADER_TYPE_PIXEL, "Material", dg::SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE).
        Build("mat::gizmo::arrow");


    m_eventHandler = eventHandler;
    m_rootNode = std::make_shared<TransformNode>();
    auto moveRoot = m_rootNode->NewChild();
    m_move->Create(device, eventHandler, material, moveRoot);

    SelectNode(m_selectedObject);

    return m_rootNode;
}

void Gizmo3D::Update(const std::shared_ptr<Camera>& camera, math::Rect windowRect, bool mouseUnderWindow, GizmoFoundDesc& foundDesc) {
    bool mouseFirstRelease = m_eventHandler->IsKeyReleasedFirstTime(Key::MouseLeft);

    float absoluteMousePosX, absoluteMousePosY;
    m_eventHandler->GetCursorPosition(absoluteMousePosX, absoluteMousePosY);
    auto mousePos = math::Point(
            static_cast<uint32_t>(absoluteMousePosX) - windowRect.x,
            static_cast<uint32_t>(absoluteMousePosY) - windowRect.y);

    foundDesc.needFound = false;
    foundDesc.mouseX = mousePos.x;
    foundDesc.mouseY = mousePos.y;
    foundDesc.windowWidth = windowRect.Width();
    foundDesc.windowHeight = windowRect.Height();

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

    if (!mouseUnderWindow) {
        return;
    }

    if (!m_move->IsMoved()) {
        m_invRayMatrix = nodeMatrix.Inverse();
    }

    auto rayStart = camera->GetPosition();
    rayStart = static_cast<dg::float3>(dg::float4(rayStart, 1.f) * m_invRayMatrix);

    auto rayDir = camera->ScreenPointToRay(mousePos, windowRect.Size());
    rayDir = dg::normalize(static_cast<dg::float3>(dg::float4(rayDir, 0.f) * m_invRayMatrix));

    m_move->Update(rayStart, rayDir);
    if (mouseFirstRelease && !m_move->IsSelected() && !m_move->IsMoved()) {
        foundDesc.needFound = true;
    }
}

void Gizmo3D::SelectNode(const std::shared_ptr<TransformNode>& node) {
    m_selectedObject = node;
    m_rootNode->SetVisible(static_cast<bool>(node));
    m_move->SelectNode(node);
}
