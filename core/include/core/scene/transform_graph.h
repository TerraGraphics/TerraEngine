#pragma once

#include <memory>
#include <vector>

#include "core/dg/dg.h"
#include "core/common/ctor.h"
#include "core/math/constants.h"

class TransformNode;
struct TransformUpdateDesc {
    uint32_t lastId;
    uint32_t findId;
    std::shared_ptr<TransformNode> findResult;
    std::vector<std::shared_ptr<TransformNode>> nodeList;
};

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

    void SetVisible(bool value) noexcept;

    void SetTransform(const dg::float4x4& transform);
    const dg::float4x4& GetBaseTransform() const noexcept { return m_baseTransform; }
    const dg::float4x4& GetWorldMatrix() const noexcept { return m_world; }
    const dg::float3x3& GetNormalMatrix() const noexcept { return m_normal; }

    uint32_t GetId() const noexcept { return m_id; }
    std::shared_ptr<MaterialNode>& GetMaterialNode() noexcept { return m_materialNode; }

    void Update(TransformUpdateDesc& value);

private:
    std::weak_ptr<TransformNode> m_parent;
    std::vector<std::shared_ptr<TransformNode>> m_children;
    std::shared_ptr<MaterialNode> m_materialNode = nullptr;
    uint32_t m_id = 0;
    bool m_isDirty = true;
    bool m_isVisible = true;
    dg::float4x4 m_baseTransform = dg::One4x4;
    dg::float4x4 m_world = dg::One4x4;
    dg::float3x3 m_normal = dg::One3x3;
};

class TransformGraph {
public:
    TransformGraph();
    ~TransformGraph() = default;

    std::shared_ptr<TransformNode> NewChild(const dg::float4x4& transform = dg::One4x4);
    std::shared_ptr<TransformNode> NewChild(const std::shared_ptr<MaterialNode>& materialNode, const dg::float4x4& transform = dg::One4x4);
    void AddChild(const std::shared_ptr<TransformNode>& node);

    void UpdateGraph(TransformUpdateDesc& value);

private:
    uint32_t m_lastId = 0;
    std::shared_ptr<TransformNode> m_root = nullptr;
};
