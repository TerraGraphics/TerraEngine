#include "core/scene/vertex_buffer.h"

#include <DiligentCore/Graphics/GraphicsEngine/interface/RenderDevice.h>
#include <DiligentCore/Graphics/GraphicsEngine/interface/DeviceContext.h>


VertexBuffer::VertexBuffer(DevicePtr& device, const void* data, uint32_t size, const dg::Char* name) {
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

void VertexBuffer::Bind(ContextPtr& context, uint32_t offset) {
    dg::IBuffer* pBuffs[] = { m_vertexBuffer };
    context->SetVertexBuffers(0, 1, pBuffs, &offset, dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION, dg::SET_VERTEX_BUFFERS_FLAG_RESET);
}
