#pragma once

#include <memory>
#include <vector>

#include "core/common/dg.h"
#include "core/common/ctor.h"
#include "core/math/constants.h"

namespace Diligent {
    #include "structures.fxh"
}

class MaterialNode;
class TransformNode : Noncopyable, public std::enable_shared_from_this<TransformNode> {
public:
    TransformNode() = default;
    TransformNode(const dg::float4x4& transform);
    TransformNode(const dg::float4x4& transform, const std::weak_ptr<TransformNode>& parent);
    ~TransformNode() = default;

    std::shared_ptr<TransformNode> Clone(const dg::float4x4& transform) const;
    std::shared_ptr<TransformNode> Clone(const std::weak_ptr<TransformNode>& parent) const;

    std::shared_ptr<TransformNode> NewChild(const dg::float4x4& transform = dg::One4x4);
    std::shared_ptr<TransformNode> NewChild(const std::shared_ptr<MaterialNode>& materialNode, const dg::float4x4& transform = dg::One4x4);

    void AddChild(const std::shared_ptr<TransformNode>& node);

    void SetTransform(const dg::float4x4& transform);
    const dg::float4x4& GetBaseTransform() const noexcept { return m_baseTransform; }
    const dg::ShaderTransform& GetTransform() const noexcept { return m_transform; }
    const std::shared_ptr<MaterialNode> GetMaterialNode() const noexcept { return m_materialNode; }

    void Update(std::vector<std::shared_ptr<TransformNode>>& nodeList);

private:
    std::weak_ptr<TransformNode> m_parent;
    std::vector<std::shared_ptr<TransformNode>> m_children;
    std::shared_ptr<MaterialNode> m_materialNode = nullptr;
    bool m_isDirty = true;
    dg::float4x4 m_baseTransform = dg::One4x4;
    dg::ShaderTransform m_transform = {dg::One4x4, dg::One4x4};
};

class TransformGraph {
public:
    TransformGraph();
    ~TransformGraph() = default;

    std::shared_ptr<TransformNode> NewChild(const dg::float4x4& transform = dg::One4x4);
    std::shared_ptr<TransformNode> NewChild(const std::shared_ptr<MaterialNode>& materialNode, const dg::float4x4& transform = dg::One4x4);
    void AddChild(const std::shared_ptr<TransformNode>& node);

    void UpdateGraph(std::vector<std::shared_ptr<TransformNode>>& nodeList);
private:
    std::shared_ptr<TransformNode> m_root = nullptr;
};
