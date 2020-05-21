#include "core/scene/transform_graph.h"

#include "core/scene/geometry.h"
#include "core/common/exception.h"
#include "core/material/material.h"
#include "core/math/normal_matrix.h"


DrawNode::DrawNode(const std::shared_ptr<Geometry>& geometry, MaterialView materialView, const dg::float4x4& worldMatrix, const dg::float3x3& normalMatrix, uint32_t id)
    : geometry(geometry)
    , materialView(materialView)
    , worldMatrix(worldMatrix)
    , normalMatrix(normalMatrix)
    , id(id) {

}

TransformNode::TransformNode(const dg::float4x4& transform)
    : m_baseTransform(transform) {

}

TransformNode::TransformNode(const dg::float4x4& transform, const std::weak_ptr<TransformNode>& parent)
    : m_parent(parent)
    , m_baseTransform(transform) {

}

TransformNode::TransformNode(const std::shared_ptr<Geometry>& geometry, const std::shared_ptr<Material>& material, const dg::float4x4& transform)
    : m_geometry(geometry)
    , m_material(material)
    , m_baseTransform(transform) {

    if (!geometry) {
        throw EngineError("TransformNode: geometry param is empty");
    }
    if (!geometry) {
        throw EngineError("TransformNode: material param is empty");
    }
}

std::shared_ptr<TransformNode> TransformNode::Clone(const dg::float4x4& transform) const {
    auto node = std::make_shared<TransformNode>(transform);
    if (m_geometry) {
        node->m_geometry = m_geometry;
        node->m_material = m_material;
    }

    node->m_children.reserve(m_children.size());
    for (const auto& child : m_children) {
        node->AddChild(child->Clone(node));
    }

    return node;
}

std::shared_ptr<TransformNode> TransformNode::Clone(const std::weak_ptr<TransformNode>& parent) const {
    auto node = std::make_shared<TransformNode>(m_baseTransform, parent);
    if (m_geometry) {
        node->m_geometry = m_geometry;
        node->m_material = m_material;
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

std::shared_ptr<TransformNode> TransformNode::NewChild(const std::shared_ptr<Geometry>& geometry, const std::shared_ptr<Material>& material, const dg::float4x4& transform) {
    auto node = std::make_shared<TransformNode>(transform, shared_from_this());
    if (!geometry) {
        throw EngineError("TransformNode: geometry param is empty");
    }
    if (!geometry) {
        throw EngineError("TransformNode: material param is empty");
    }

    node->m_geometry = geometry;
    node->m_material = material;
    m_children.push_back(node);

    return node;
}

void TransformNode::AddChild(const std::shared_ptr<TransformNode>& node) {
    node->m_parent = shared_from_this();
    m_children.push_back(node);
}

void TransformNode::SetVisible(bool value) noexcept {
    m_isVisible = value;
}

void TransformNode::SetTransform(const dg::float4x4& transform) {
    m_isDirty = true;
    m_baseTransform = transform;
}

void TransformNode::Update(TransformUpdateDesc& nodeList, bool isDirty) {
    if (!m_isVisible) {
        return;
    }

    if (m_isDirty || isDirty) {
        isDirty = true;
        if (auto parent = m_parent.lock()) {
            m_world = m_baseTransform * parent->m_world;
            m_normal = MakeNormalMatrix3x3(m_world);
            m_isDirty = false;
        }
    }

    if (m_geometry) {
        if (m_id == 0) {
            m_id = ++nodeList.lastId;
        }
        if (m_id == nodeList.findId) {
            nodeList.findResult = shared_from_this();
        }
        nodeList.nodeList.emplace_back(
            m_geometry, m_material->GetView(nodeList.frameNum, nodeList.targetsId, m_geometry->GetVDeclId(), nodeList.vDeclIdPerInstance),
            m_world, m_normal, m_id);
    }

    for (auto& node : m_children) {
        node->Update(nodeList, isDirty);
    }
}

TransformGraph::TransformGraph()
    : m_root(std::make_shared<TransformNode>(dg::One4x4, std::make_shared<TransformNode>())) {

}

std::shared_ptr<TransformNode> TransformGraph::NewChild(const dg::float4x4& transform) {
    return m_root->NewChild(transform);
}

std::shared_ptr<TransformNode> TransformGraph::NewChild(const std::shared_ptr<Geometry>& geometry, const std::shared_ptr<Material>& material, const dg::float4x4& transform) {
    return m_root->NewChild(geometry, material, transform);
}

void TransformGraph::AddChild(const std::shared_ptr<TransformNode>& node) {
    m_root->AddChild(node);
}

void TransformGraph::UpdateGraph(TransformUpdateDesc& nodeList) {
    nodeList.frameNum = m_frameNum++;
    nodeList.lastId = m_lastId;
    m_root->Update(nodeList, false);
    m_lastId = nodeList.lastId;
}
