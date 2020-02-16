#include "middleware/gizmo/gizmo_3d.h"

#include "core/scene/transform_graph.h"
#include "core/material/material_builder.h"
#include "middleware/generator/generator.h"
#include "middleware/std_material/std_material.h"


std::shared_ptr<TransformNode> Gizmo3D::Create(DevicePtr& device, std::shared_ptr<MaterialBuilder>& materialBuilder,
    const VertexDecl& additionalVertexDecl) {

    auto material = materialBuilder->Create(materialBuilder->GetShaderMask("BASE_COLOR_MATERIAL"), VertexPNC::GetDecl(), additionalVertexDecl).
        DepthEnable(false).
        CullMode(dg::CULL_MODE_NONE).
        Var(dg::SHADER_TYPE_PIXEL, "Material", dg::SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE).
        Build("mat::gizmo::arrow");

    m_rootNode = std::make_shared<TransformNode>();
    m_transformNode = m_rootNode->NewChild();

    for (const auto axis: {Axis::X, Axis::Y, Axis::Z}) {
        auto translation = dg::float3(0, 0, 0);

        float coneHeight = 0.1f;
        ConeShape coneShape({10, 1}, axis, 0.03f, coneHeight);
        translation[static_cast<uint>(axis)] = 1.f - coneHeight * 0.5f;
        coneShape.SetTranform(dg::float4x4::Translation(translation));

        float cylinderSpacing = 0.05f;
        float cylinderHeight = 1.f - coneHeight - cylinderSpacing;
        CylinderShape cylinderShape({5, 1}, axis, 0.01f, cylinderHeight);
        translation[static_cast<uint>(axis)] = cylinderSpacing + cylinderHeight * 0.5f;
        cylinderShape.SetTranform(dg::float4x4::Translation(translation));

        auto arrowNode = std::make_shared<StdMaterial>(material, ShapeBuilder(device).Join({&coneShape, &cylinderShape}, "arrow"));
        auto color = dg::float4(0.f, 0.f, 0.f, 1.f);
        color[static_cast<uint>(axis)] = 1.0f;
        arrowNode->SetBaseColor(color);

        m_transformNode->NewChild(arrowNode);
    }

    return m_rootNode;
}
