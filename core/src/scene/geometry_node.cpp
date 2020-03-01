#include "core/scene/geometry_node.h"

#include "core/dg/graphics_types.h"
#include "core/dg/device_context.h"
#include "core/scene/index_buffer.h"
#include "core/scene/vertex_buffer.h"

GeometryNode::GeometryNode() {

}

GeometryNode::~GeometryNode() {

}

GeometryNodeUnindexed::GeometryNodeUnindexed(const std::shared_ptr<VertexBuffer>& vb, uint32_t vbOffsetBytes, uint32_t vbCount)
    : m_vertexBuffer(vb)
    , m_vertexBufferOffsetBytes(vbOffsetBytes)
    , m_vertexBufferCount(vbCount) {

}

GeometryNodeUnindexed::~GeometryNodeUnindexed() {

}

void GeometryNodeUnindexed::Bind(ContextPtr& context) {
    m_vertexBuffer->Bind(context, m_vertexBufferOffsetBytes);
}

uint32_t GeometryNodeUnindexed::Draw(ContextPtr& context, uint32_t firstInstanceIndex) {
    dg::DrawAttribs drawAttrs;
    drawAttrs.NumVertices = m_vertexBufferCount;
    drawAttrs.FirstInstanceLocation = firstInstanceIndex;
    drawAttrs.Flags = dg::DRAW_FLAG_VERIFY_ALL;

    context->Draw(drawAttrs);

    // TODO: fix for not triangle
    return m_vertexBufferCount / 3;
}

GeometryNodeIndexed::GeometryNodeIndexed(const std::shared_ptr<VertexBuffer>& vb, uint32_t vbOffsetBytes,
    const std::shared_ptr<IndexBuffer>& ib, uint32_t ibOffsetBytes, uint32_t ibCount, bool ibUint32)
    : GeometryNode()
    , m_vertexBuffer(vb)
    , m_indexBuffer(ib)
    , m_vertexBufferOffsetBytes(vbOffsetBytes)
    , m_indexBufferOffsetBytes(ibOffsetBytes)
    , m_indexBufferCount(ibCount)
    , m_indexBufferUint32(ibUint32) {

}

GeometryNodeIndexed::~GeometryNodeIndexed() {

}

void GeometryNodeIndexed::Bind(ContextPtr& context) {
    m_vertexBuffer->Bind(context, m_vertexBufferOffsetBytes);
    m_indexBuffer->Bind(context, m_indexBufferOffsetBytes);
}

uint32_t GeometryNodeIndexed::Draw(ContextPtr& context, uint32_t firstInstanceIndex) {
    dg::DrawIndexedAttribs drawAttrs;
    drawAttrs.IndexType  = m_indexBufferUint32 ? dg::VT_UINT32 : dg::VT_UINT16;
    drawAttrs.NumIndices = m_indexBufferCount;
    drawAttrs.FirstInstanceLocation = firstInstanceIndex;
    drawAttrs.Flags = dg::DRAW_FLAG_VERIFY_ALL;

    context->DrawIndexed(drawAttrs);

    // TODO: fix for not triangle
    return m_indexBufferCount / 3;
}