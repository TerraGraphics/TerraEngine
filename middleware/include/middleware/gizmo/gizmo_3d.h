#pragma once

#include "core/dg/dg.h"
#include "core/material/vertex_decl.h"


class Camera;
class Material;
class GizmoMove;
class TransformNode;
class MaterialBuilder;
class DefaultWindowEventsHandler;
struct GizmoFoundDesc {
    bool needFound = false;
    uint32_t mouseX = 0;
    uint32_t mouseY = 0;
    uint32_t windowWidth = 0;
    uint32_t windowHeight = 0;
};

class Gizmo3D : Fixed {
public:
    Gizmo3D();
    ~Gizmo3D();

public:
    std::shared_ptr<TransformNode> Create(DevicePtr& device, const std::shared_ptr<DefaultWindowEventsHandler>& eventHandler,
        std::shared_ptr<MaterialBuilder>& materialBuilder, const VertexDecl& additionalVertexDecl);
    void Update(const std::shared_ptr<Camera>& camera, math::Rect windowRect, bool mouseUnderWindow, GizmoFoundDesc& foundDesc);
    void SelectNode(const std::shared_ptr<TransformNode>& node);

private:
    dg::float4x4 m_invRayMatrix;
    std::unique_ptr<GizmoMove> m_move;
    std::shared_ptr<TransformNode> m_rootNode = nullptr;
    std::shared_ptr<TransformNode> m_selectedObject = nullptr;
    std::shared_ptr<DefaultWindowEventsHandler> m_eventHandler = nullptr;
};
