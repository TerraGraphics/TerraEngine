#pragma once

#include "core/material/vertex_decl.h"


class TransformNode;
class MaterialBuilder;
class Gizmo3D : Fixed {
public:
    Gizmo3D() = default;
    ~Gizmo3D() = default;

public:
    std::shared_ptr<TransformNode> Create(DevicePtr& device, std::shared_ptr<MaterialBuilder>& materialBuilder,
        const VertexDecl& additionalVertexDecl);

    void SelectNode(std::shared_ptr<TransformNode> node);

private:
    std::shared_ptr<TransformNode> m_rootNode;
    std::shared_ptr<TransformNode> m_transformNode;
    std::shared_ptr<TransformNode> m_selectedObject;
};
