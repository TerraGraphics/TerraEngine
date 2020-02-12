#include "core/scene/vertex_buffer.h"

#include "core/common/exception.h"
#include "core/dg/render_device.h"
#include "core/dg/device_context.h"


VertexBuffer::VertexBuffer(DevicePtr& device, const void* data, uint32_t size, const dg::Char* name) {
    dg::BufferDesc buffDesc;
    buffDesc.Name = name;
    buffDesc.Usage = dg::USAGE_STATIC;
    buffDesc.BindFlags = dg::BIND_VERTEX_BUFFER;
    buffDesc.CPUAccessFlags = dg::CPU_ACCESS_NONE;
    buffDesc.uiSizeInBytes = size;

    dg::BufferData buffData;
    buffData.pData = data;
    buffData.DataSize = size;
    device->CreateBuffer(buffDesc, &buffData, &m_vertexBuffer);
}

void VertexBuffer::Bind(ContextPtr& context, uint32_t offset) {
    dg::IBuffer* pBuffs[] = { m_vertexBuffer };
    context->SetVertexBuffers(0, 1, pBuffs, &offset, dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION, dg::SET_VERTEX_BUFFERS_FLAG_NONE);
}

WriteableVertexBuffer::WriteableVertexBuffer(DevicePtr& device, uint32_t size, dg::USAGE usage, const dg::Char* name)
    : VertexBuffer() {

    dg::BufferDesc buffDesc;
    buffDesc.Name = name;
    buffDesc.Usage = usage;
    buffDesc.BindFlags = dg::BIND_VERTEX_BUFFER;
    buffDesc.CPUAccessFlags = dg::CPU_ACCESS_WRITE;
    buffDesc.uiSizeInBytes = size;

    device->CreateBuffer(buffDesc, nullptr, &m_vertexBuffer);
}

void* WriteableVertexBuffer::MapRaw(ContextPtr& context) {
    if (m_mapped) {
        auto* name = m_vertexBuffer->GetDesc().Name;
        throw EngineError("double map for vertex buffer {}", name != nullptr ? name : "<unknown name>");
    }
    m_mapped = true;
    void* data = nullptr;
    context->MapBuffer(m_vertexBuffer, dg::MAP_WRITE, dg::MAP_FLAG_DISCARD, data);

    return data;
}

void WriteableVertexBuffer::Unmap(ContextPtr& context) {
    if (!m_mapped) {
        auto* name = m_vertexBuffer->GetDesc().Name;
        throw EngineError("unmap without map for vertex buffer {}", name != nullptr ? name : "<unknown name>");
    }

    context->UnmapBuffer(m_vertexBuffer, dg::MAP_WRITE);
    m_mapped = false;
}

void WriteableVertexBuffer::Update(ContextPtr& context, const void* data) {
    context->UpdateBuffer(m_vertexBuffer, 0, m_vertexBuffer->GetDesc().uiSizeInBytes, data, dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
}

void WriteableVertexBuffer::BindExclusively(ContextPtr& context, uint32_t slot) {
    uint32_t offset = 0;
    dg::IBuffer* pBuffs[] = { m_vertexBuffer };
    context->SetVertexBuffers(slot, 1, pBuffs, &offset, dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION, dg::SET_VERTEX_BUFFERS_FLAG_RESET);
}
