#pragma once

#include <memory>
#include <cstdint>

#include "core/math/types.h"
#include "core/common/ctor.h"


class Camera;
class IGizmo;
class TransformNode;
class DefaultWindowEventsHandler;
struct GizmoFoundDesc {
    bool needFound = false;
    uint32_t mouseX = 0;
    uint32_t mouseY = 0;
};

class Gizmo3D : Fixed {
public:
    enum class Type : uint8_t {
        MOVE = 0,
        ROTATE = 1,
        SCALE = 2,
    };
public:
    Gizmo3D();
    ~Gizmo3D();

public:
    std::shared_ptr<TransformNode> Create();
    void Update(const std::shared_ptr<Camera>& camera, math::RectF windowRect, bool mouseUnderWindow, GizmoFoundDesc& foundDesc);
    void SelectNode(const std::shared_ptr<TransformNode>& node);

    Type GetType() const noexcept { return m_type; }
    void SetType(Type value);

private:
    Type m_type = Type::MOVE;
    dg::double4x4 m_invRayMatrix;
    std::unique_ptr<IGizmo> m_gizmos[3];
    IGizmo* m_activeGizmo = nullptr;
    std::shared_ptr<TransformNode> m_rootNode = nullptr;
    std::shared_ptr<TransformNode> m_selectedObject = nullptr;
    std::shared_ptr<DefaultWindowEventsHandler> m_eventHandler = nullptr;
};
