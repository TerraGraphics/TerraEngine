#pragma once

#include "core/material/vertex_decl.h"


class Camera;
class Material;
class TransformNode;
class MaterialBuilder;
class DefaultWindowEventsHandler;
class GizmoMove : Fixed {
public:
    GizmoMove() = default;
    ~GizmoMove() = default;

public:
    void Create(DevicePtr& device, const std::shared_ptr<DefaultWindowEventsHandler>& eventHandler,
        std::shared_ptr<Material>& material, std::shared_ptr<TransformNode>& root);
    void Update(dg::float3 rayStart, dg::float3 rayDir);

    bool IsActive() const noexcept { return m_isSelected || m_isMoved; }

private:
    void SelectReset();
    void SelectAxis(math::Axis value);
    bool FindSelect(dg::float3 rayStart, dg::float3 rayDir, math::Axis& result);

private:
    bool m_isSelected = false;
    bool m_isMoved = false;
    std::shared_ptr<TransformNode> m_root;
    std::shared_ptr<TransformNode> m_arrowNodes[3];
    std::shared_ptr<DefaultWindowEventsHandler> m_eventHandler;

    static constexpr float m_arrowRadius = 0.01f;
    static constexpr float m_arrowActiveRadius = 0.04f;
    static constexpr float m_arrowSelectScale = 1.5f;
};

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

    void SelectNode(std::shared_ptr<TransformNode> node);

private:
    std::unique_ptr<GizmoMove> m_move;
    std::shared_ptr<TransformNode> m_rootNode = nullptr;
    std::shared_ptr<TransformNode> m_selectedObject = nullptr;
    std::shared_ptr<DefaultWindowEventsHandler> m_eventHandler = nullptr;
};
