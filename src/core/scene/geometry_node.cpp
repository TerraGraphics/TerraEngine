#include "core/scene/geometry_node.h"

#include <DiligentCore/Graphics/GraphicsEngine/interface/DeviceContext.h>


GeometryNode::GeometryNode(const std::shared_ptr<VertexBuffer>& vb, uint32_t vbOffsetBytes,
    const std::shared_ptr<IndexBuffer>& ib, uint32_t ibOffsetBytes, uint32_t ibCount, bool ibUint32)
    : m_vertexBuffer(vb)
    , m_indexBuffer(ib)
    , m_vertexBufferOffsetBytes(vbOffsetBytes)
    , m_indexBufferOffsetBytes(ibOffsetBytes)
    , m_indexBufferCount(ibCount)
    , m_indexBufferUint32(ibUint32) {

}

void GeometryNode::Bind(ContextPtr& context) {
    m_vertexBuffer->Bind(context, m_vertexBufferOffsetBytes);
    m_indexBuffer->Bind(context, m_indexBufferOffsetBytes);
}

uint32_t GeometryNode::Draw(ContextPtr& context, uint32_t firstInstanceIndex) {
    dg::DrawIndexedAttribs drawAttrs;
    drawAttrs.IndexType  = m_indexBufferUint32 ? dg::VT_UINT32 : dg::VT_UINT16;
    drawAttrs.NumIndices = m_indexBufferCount;
    drawAttrs.FirstInstanceLocation = firstInstanceIndex;
    drawAttrs.Flags = dg::DRAW_FLAG_VERIFY_ALL;

    context->DrawIndexed(drawAttrs);

    return m_indexBufferCount / 3;
}
