#include "core/scene/index_buffer.h"

#include "core/dg/buffer.h"
#include "core/dg/render_device.h"
#include "core/dg/graphics_types.h"
#include "core/dg/device_context.h"


IndexBuffer::IndexBuffer(DevicePtr& device, const void* data, uint32_t size, const char* name) {
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

IndexBuffer::~IndexBuffer() {

}

void IndexBuffer::Bind(ContextPtr& context, uint32_t offset) {
    context->SetIndexBuffer(m_indexBuffer, offset, dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
}
