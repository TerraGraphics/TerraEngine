#pragma once

#include <memory>


class TransformNode;
class IGizmo {
public:
    virtual ~IGizmo() = default;

    virtual bool IsSelected() const noexcept = 0;
    virtual bool IsMoved() const noexcept = 0;

    virtual void Update(dg::float3 rayStart, dg::float3 rayDir) = 0;
    virtual void SetEnable(bool value) = 0;
    virtual void SelectNode(const std::shared_ptr<TransformNode>& node) = 0;
};
