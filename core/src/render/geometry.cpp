#include "core/render/geometry.h"

#include "dg/context.h"
#include "dg/graphics_types.h"
#include "core/render/index_buffer.h"
#include "core/render/vertex_buffer.h"


Geometry::Geometry(uint16_t vDeclId)
    : m_vDeclId(vDeclId) {

}

Geometry::~Geometry() {

}

GeometryUnindexed::GeometryUnindexed(const std::shared_ptr<VertexBuffer>& vb, uint32_t vbOffsetBytes, uint32_t vbCount, uint16_t vDeclId)
    : Geometry(vDeclId)
    , m_vertexBuffer(vb)
    , m_vertexBufferOffsetBytes(vbOffsetBytes)
    , m_vertexBufferCount(vbCount) {

}

GeometryUnindexed::~GeometryUnindexed() {

}

void GeometryUnindexed::Bind(ContextPtr& context) {
    m_vertexBuffer->Bind(context, m_vertexBufferOffsetBytes);
}

uint32_t GeometryUnindexed::Draw(ContextPtr& context, uint32_t firstInstanceIndex) {
    dg::DrawAttribs drawAttrs;
    drawAttrs.NumVertices = m_vertexBufferCount;
    drawAttrs.FirstInstanceLocation = firstInstanceIndex;
    drawAttrs.Flags = dg::DRAW_FLAG_VERIFY_ALL;

    context->Draw(drawAttrs);

    // TODO: fix for not triangle
    return m_vertexBufferCount / 3;
}

GeometryIndexed::GeometryIndexed(const std::shared_ptr<VertexBuffer>& vb, uint32_t vbOffsetBytes,
    const std::shared_ptr<IndexBuffer>& ib, uint32_t ibOffsetBytes, uint32_t ibCount, bool ibUint32, uint16_t vDeclId)
    : Geometry(vDeclId)
    , m_vertexBuffer(vb)
    , m_indexBuffer(ib)
    , m_vertexBufferOffsetBytes(vbOffsetBytes)
    , m_indexBufferOffsetBytes(ibOffsetBytes)
    , m_indexBufferCount(ibCount)
    , m_indexBufferUint32(ibUint32) {

}

GeometryIndexed::~GeometryIndexed() {

}

void GeometryIndexed::Bind(ContextPtr& context) {
    m_vertexBuffer->Bind(context, m_vertexBufferOffsetBytes);
    m_indexBuffer->Bind(context, m_indexBufferOffsetBytes);
}

uint32_t GeometryIndexed::Draw(ContextPtr& context, uint32_t firstInstanceIndex) {
    dg::DrawIndexedAttribs drawAttrs;
    drawAttrs.IndexType  = m_indexBufferUint32 ? dg::VT_UINT32 : dg::VT_UINT16;
    drawAttrs.NumIndices = m_indexBufferCount;
    drawAttrs.FirstInstanceLocation = firstInstanceIndex;
    drawAttrs.Flags = dg::DRAW_FLAG_VERIFY_ALL;

    context->DrawIndexed(drawAttrs);

    // TODO: fix for not triangle
    return m_indexBufferCount / 3;
}
