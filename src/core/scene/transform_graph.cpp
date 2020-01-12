#include "core/scene/transform_graph.h"

#include "core/math/normal_matrix.h"
#include "core/scene/material_node.h"


TransformNode::TransformNode(const dg::float4x4& transform)
    : m_baseTransform(transform) {

}

TransformNode::TransformNode(const dg::float4x4& transform, const std::weak_ptr<TransformNode>& parent)
    : m_parent(parent)
    , m_baseTransform(transform) {

}

std::shared_ptr<TransformNode> TransformNode::Clone(const dg::float4x4& transform) const {
    auto node = std::make_shared<TransformNode>(transform);
    if (m_materialNode) {
        node->m_materialNode = m_materialNode;
    }

    node->m_children.reserve(m_children.size());
    for (const auto& child : m_children) {
        node->AddChild(child->Clone(node));
    }

    return node;
}

std::shared_ptr<TransformNode> TransformNode::Clone(const std::weak_ptr<TransformNode>& parent) const {
    auto node = std::make_shared<TransformNode>(m_baseTransform, parent);
    if (m_materialNode) {
        node->m_materialNode = m_materialNode;
    }

    node->m_children.reserve(m_children.size());
    for (const auto& child : m_children) {
        node->AddChild(child->Clone(node));
    }

    return node;
}

std::shared_ptr<TransformNode> TransformNode::NewChild(const dg::float4x4& transform) {
    auto node = std::make_shared<TransformNode>(transform, shared_from_this());
    m_children.push_back(node);

    return node;
}

std::shared_ptr<TransformNode> TransformNode::NewChild(const std::shared_ptr<MaterialNode>& materialNode, const dg::float4x4& transform) {
    auto node = std::make_shared<TransformNode>(transform, shared_from_this());
    node->m_materialNode = materialNode;
    m_children.push_back(node);

    return node;
}

void TransformNode::AddChild(const std::shared_ptr<TransformNode>& node) {
    node->m_parent = shared_from_this();
    m_children.push_back(node);
}

void TransformNode::SetTransform(const dg::float4x4& transform) {
    m_isDirty = true;
    m_baseTransform = transform;
}

void TransformNode::Update(std::vector<std::shared_ptr<TransformNode>>& nodeList) {
    if (m_isDirty) {
        if (auto parent = m_parent.lock()) {
            m_world = m_baseTransform * parent->m_world;
            m_normal = MakeNormalMatrix3x3(m_world);
            m_isDirty = false;
        }
    }

    if (m_materialNode) {
        nodeList.push_back(shared_from_this());
    }

    for (auto& node : m_children) {
        node->Update(nodeList);
    }
}

TransformGraph::TransformGraph()
    : m_root(std::make_shared<TransformNode>(dg::One4x4, std::make_shared<TransformNode>())) {

}

std::shared_ptr<TransformNode> TransformGraph::NewChild(const dg::float4x4& transform) {
    return m_root->NewChild(transform);
}

std::shared_ptr<TransformNode> TransformGraph::NewChild(const std::shared_ptr<MaterialNode>& materialNode, const dg::float4x4& transform) {
    return m_root->NewChild(materialNode, transform);
}

void TransformGraph::AddChild(const std::shared_ptr<TransformNode>& node) {
    m_root->AddChild(node);
}

void TransformGraph::UpdateGraph(std::vector<std::shared_ptr<TransformNode>>& nodeList) {
    m_root->Update(nodeList);
}
