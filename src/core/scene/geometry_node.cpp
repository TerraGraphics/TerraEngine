#include "core/scene/geometry_node.h"

#include <DiligentCore/Graphics/GraphicsEngine/interface/RenderDevice.h>
#include <DiligentCore/Graphics/GraphicsEngine/interface/DeviceContext.h>


uint32_t Counter::m_lastId = 0;

VertexBuffer::VertexBuffer(DevicePtr& device, const void* data, size_t size, const dg::Char* name) {
    dg::BufferDesc buffDesc;
    buffDesc.Name = name;
    buffDesc.Usage = dg::USAGE_STATIC;
    buffDesc.BindFlags = dg::BIND_VERTEX_BUFFER;
    buffDesc.uiSizeInBytes = size;

    dg::BufferData buffData;
    buffData.pData = data;
    buffData.DataSize = size;
    device->CreateBuffer(buffDesc, &buffData, &m_vertexBuffer);
}

VertexBuffer::VertexBuffer(VertexBuffer&& other) noexcept
    : m_vertexBuffer(std::move(other.m_vertexBuffer)) {

    other.m_vertexBuffer = nullptr;
}

VertexBuffer& VertexBuffer::operator=(VertexBuffer&& other) noexcept {
    m_vertexBuffer.swap(other.m_vertexBuffer);

    return *this;
}

void VertexBuffer::Bind(ContextPtr& context) {
    dg::Uint32 offset = 0;
    dg::IBuffer* pBuffs[] = { m_vertexBuffer };
    context->SetVertexBuffers(0, 1, pBuffs, &offset, dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION, dg::SET_VERTEX_BUFFERS_FLAG_RESET);
}

IndexBuffer::IndexBuffer(DevicePtr& device, const uint16_t* data, size_t size, const dg::Char* name)
    : m_number(size / sizeof(uint16_t))
    , m_type(dg::VALUE_TYPE::VT_UINT16) {

    dg::BufferDesc buffDesc;
    buffDesc.Name = name;
    buffDesc.Usage = dg::USAGE_STATIC;
    buffDesc.BindFlags = dg::BIND_INDEX_BUFFER;
    buffDesc.uiSizeInBytes = size;

    dg::BufferData buffData;
    buffData.pData = data;
    buffData.DataSize = size;
    device->CreateBuffer(buffDesc, &buffData, &m_indexBuffer);
}

IndexBuffer::IndexBuffer(DevicePtr& device, const uint32_t* data, size_t size, const dg::Char* name)
    : m_number(size / sizeof(uint32_t))
    , m_type(dg::VALUE_TYPE::VT_UINT32) {

    dg::BufferDesc buffDesc;
    buffDesc.Name = name;
    buffDesc.Usage = dg::USAGE_STATIC;
    buffDesc.BindFlags = dg::BIND_INDEX_BUFFER;
    buffDesc.uiSizeInBytes = size;

    dg::BufferData buffData;
    buffData.pData = data;
    buffData.DataSize = size;
    device->CreateBuffer(buffDesc, &buffData, &m_indexBuffer);
}

IndexBuffer::IndexBuffer(IndexBuffer&& other) noexcept
    : m_indexBuffer(std::move(other.m_indexBuffer)) {

    std::swap(m_number, other.m_number);
    std::swap(m_type, other.m_type);
    other.m_indexBuffer = nullptr;
}

IndexBuffer& IndexBuffer::operator=(IndexBuffer&& other) noexcept {
    std::swap(m_number, other.m_number);
    std::swap(m_type, other.m_type);
    m_indexBuffer.swap(other.m_indexBuffer);

    return *this;
}

void IndexBuffer::Bind(ContextPtr& context) {
    context->SetIndexBuffer(m_indexBuffer, 0, dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
}

GeometryNode::GeometryNode(VertexBuffer&& vertexBuffer, IndexBuffer&& indexBuffer)
    : m_vertexBuffer{std::move(vertexBuffer)}
    , m_indexBuffer{std::move(indexBuffer)} {

}

void GeometryNode::Bind(ContextPtr& context) {
    m_vertexBuffer.Bind(context);
    m_indexBuffer.Bind(context);
}

uint32_t GeometryNode::Draw(ContextPtr& context) {
    dg::DrawIndexedAttribs drawAttrs;
    drawAttrs.IndexType  = m_indexBuffer.GetType();
    drawAttrs.NumIndices = m_indexBuffer.GetNumber();
    drawAttrs.Flags = dg::DRAW_FLAG_VERIFY_ALL;

    context->DrawIndexed(drawAttrs);

    return m_indexBuffer.GetNumber() / 3;
}
