#pragma once

#include "core/material/vertex_decl.h"


class Camera;
class Material;
class TransformNode;
class MaterialBuilder;
class GizmoMove : Fixed {
public:
    GizmoMove() = default;
    ~GizmoMove() = default;

public:
    void Create(DevicePtr& device, std::shared_ptr<Material>& material, std::shared_ptr<TransformNode>& root);
    void Select(dg::float3 rayStart, dg::float3 rayDir);

private:
    std::shared_ptr<TransformNode> m_root;
    std::shared_ptr<TransformNode> m_arrowNodes[3];

    static constexpr float m_arrowRadius = 0.01f;
    static constexpr float m_arrowActiveRadius = 0.04f;
    static constexpr float m_arrowSelectScale = 1.5f;
};

class Gizmo3D : Fixed {
public:
    Gizmo3D();
    ~Gizmo3D();

public:
    std::shared_ptr<TransformNode> Create(DevicePtr& device, std::shared_ptr<MaterialBuilder>& materialBuilder,
        const VertexDecl& additionalVertexDecl);
    void Update(const std::shared_ptr<Camera>& camera, math::Size sceenSize, bool mouseUnderWindow, math::Point mousePos);

    void SelectNode(std::shared_ptr<TransformNode> node);

private:
    std::unique_ptr<GizmoMove> m_move;
    std::shared_ptr<TransformNode> m_rootNode;
    std::shared_ptr<TransformNode> m_selectedObject;
};
