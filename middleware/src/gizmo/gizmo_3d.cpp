#include "middleware/gizmo/gizmo_3d.h"

#include "core/camera/camera.h"
#include "core/math/advanced.h"
#include "core/scene/transform_graph.h"
#include "core/material/material_builder.h"
#include "middleware/generator/generator.h"
#include "middleware/std_material/std_material.h"


void GizmoMove::Create(DevicePtr& device, std::shared_ptr<Material>& material, std::shared_ptr<TransformNode>& root) {
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

void GizmoMove::Select(dg::float3 rayStart, dg::float3 rayDir) {
    m_arrowNodes[static_cast<uint>(math::Axis::X)]->SetTransform(dg::One4x4);
    m_arrowNodes[static_cast<uint>(math::Axis::Y)]->SetTransform(dg::One4x4);
    m_arrowNodes[static_cast<uint>(math::Axis::Z)]->SetTransform(dg::One4x4);

    if (math::IntersectionRayAndCylinder(rayStart, rayDir, math::Axis::X, m_arrowActiveRadius, 1.f)) {
        m_arrowNodes[static_cast<uint>(math::Axis::X)]->SetTransform(dg::float4x4::Scale(1.f, m_arrowSelectScale, m_arrowSelectScale));
    } else if (math::IntersectionRayAndCylinder(rayStart, rayDir, math::Axis::Y, m_arrowActiveRadius, 1.f)) {
        m_arrowNodes[static_cast<uint>(math::Axis::Y)]->SetTransform(dg::float4x4::Scale(m_arrowSelectScale, 1.f, m_arrowSelectScale));
    } else if (math::IntersectionRayAndCylinder(rayStart, rayDir, math::Axis::Z, m_arrowActiveRadius, 1.f)) {
        m_arrowNodes[static_cast<uint>(math::Axis::Z)]->SetTransform(dg::float4x4::Scale(m_arrowSelectScale, m_arrowSelectScale, 1.f));
    }
}

Gizmo3D::Gizmo3D()
    : m_move(new GizmoMove()) {

}

Gizmo3D::~Gizmo3D() {
    m_move.reset();
}

std::shared_ptr<TransformNode> Gizmo3D::Create(DevicePtr& device, std::shared_ptr<MaterialBuilder>& materialBuilder,
    const VertexDecl& additionalVertexDecl) {

    auto material = materialBuilder->Create(materialBuilder->GetShaderMask("BASE_COLOR_MATERIAL"), VertexPNC::GetDecl(), additionalVertexDecl).
        DepthEnable(false).
        CullMode(dg::CULL_MODE_NONE).
        Var(dg::SHADER_TYPE_PIXEL, "Material", dg::SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE).
        Build("mat::gizmo::arrow");


    m_rootNode = std::make_shared<TransformNode>();
    auto moveRoot = m_rootNode->NewChild();
    m_move->Create(device, material, moveRoot);

    SelectNode(m_selectedObject);

    return m_rootNode;
}

void Gizmo3D::Update(const std::shared_ptr<Camera>& camera, math::Size sceenSize, bool mouseUnderWindow, math::Point mousePos) {
    if (!m_selectedObject) {
        return;
    }

    auto nodeMatrix = m_selectedObject->GetWorldMatrix();
    auto nodePos = dg::float3(nodeMatrix._41, nodeMatrix._42, nodeMatrix._43);
    auto dir = dg::normalize(nodePos - camera->GetPosition());
    auto gizmoPos = camera->GetPosition() + dir * 6.f;

    nodeMatrix._11 = 1.f;
    nodeMatrix._22 = 1.f;
    nodeMatrix._33 = 1.f;
    nodeMatrix._41 = gizmoPos.x;
    nodeMatrix._42 = gizmoPos.y;
    nodeMatrix._43 = gizmoPos.z;
    nodeMatrix._44 = 1.f;
    m_rootNode->SetTransform(nodeMatrix);

    if (mouseUnderWindow) {
        auto invNodeMatrix = nodeMatrix.Inverse();

        auto rayStart = camera->GetPosition();
        auto rayPoint = rayStart + camera->ScreenPointToRay(mousePos, sceenSize);

        rayStart = static_cast<dg::float3>(dg::float4(rayStart, 1.f) * invNodeMatrix);
        rayPoint = static_cast<dg::float3>(dg::float4(rayPoint, 1.f) * invNodeMatrix);
        m_move->Select(rayStart, dg::normalize(rayPoint - rayStart));
    }
}

void Gizmo3D::SelectNode(std::shared_ptr<TransformNode> node) {
    m_selectedObject = node;
    m_rootNode->SetVisible(static_cast<bool>(node));
}
