#include "core/material/texture.h"

#include "core/dg/device.h"
#include "core/dg/context.h"
#include "core/common/exception.h"


namespace {
    static void CheckIsCorrectTextureSize(uint32_t width,  uint32_t height) {
        if ((width == 0) || (height == 0)) {
            throw EngineError("DynamicTexture: size of texture must be non-zero");
        }
    }
}

DynamicTexture::LockHelper::LockHelper(DevicePtr& device, ContextPtr& context, TexturePtr& texture)
    : width(texture->GetDesc().Width)
    , height(texture->GetDesc().Height)
    , m_context(context)
    , m_texture(texture) {

    const auto& caps = device->GetDeviceCaps();
    if (caps.IsGLDevice()) {
        m_updateData = new uint8_t[width * height * 4];
        data = m_updateData;
        stride = width * 4;

    } else if (caps.IsVulkanDevice()) {
        const uint32_t mipLevel = 0;
        const uint32_t arraySlice = 0;
        dg::MappedTextureSubresource texData;
        m_context->MapTextureSubresource(m_texture, mipLevel, arraySlice, dg::MAP_WRITE, dg::MAP_FLAG_DISCARD, nullptr, texData);
        if (texData.pData == nullptr) {
            throw EngineError("DynamicTexture: failed to lock texture for write");
        }

        m_updateData = nullptr;
        data = reinterpret_cast<uint8_t*>(texData.pData);
        stride = texData.Stride;

    } else {
        throw EngineError("DynamicTexture: failed to lock texture for write, unsupported device type {}", caps.DevType);
    }
}

DynamicTexture::LockHelper::~LockHelper() {
    const uint32_t mipLevel = 0;
    const uint32_t arraySlice = 0;

    if (m_updateData != nullptr) {
        dg::Box DstBox;
        DstBox.MinX = 0;
        DstBox.MinY = 0;
        DstBox.MaxX = width;
        DstBox.MaxY = height;

        dg::TextureSubResData subresData;
        subresData.Stride = stride;
        subresData.pData  = data;

        m_context->UpdateTexture(m_texture, mipLevel, arraySlice, DstBox, subresData, dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION, dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
        delete[] m_updateData;
    } else {
        m_context->UnmapTextureSubresource(m_texture, mipLevel, arraySlice);
    }
    m_context->GenerateMips(m_texture->GetDefaultView(dg::TEXTURE_VIEW_SHADER_RESOURCE));
}

DynamicTexture::DynamicTexture(dg::IReferenceCounters* refCounters, DevicePtr& device, const dg::TextureDesc& desc)
    : dg::ObjectBase<dg::IObject>(refCounters)
    , m_device(device) {

    CheckIsCorrectTextureSize(desc.Width, desc.Height);
    m_device->CreateTexture(desc, nullptr, &m_texture);
    if (!m_texture) {
        throw EngineError("DynamicTexture: failed to create texture");
    }
}

DynamicTexture::~DynamicTexture() {
    m_texture.Release();
    m_device.Release();
}

TexturePtr DynamicTexture::Get() const noexcept {
    return m_texture;
}

bool DynamicTexture::SetSize(uint32_t width,  uint32_t height) {
    auto desc = m_texture->GetDesc();

    if ((desc.Width == width) && (desc.Height == height)) {
        return false;
    }

    desc.Width = width;
    desc.Height = height;
    CheckIsCorrectTextureSize(width, height);
    m_device->CreateTexture(desc, nullptr, &m_texture);
    if (!m_texture) {
        throw EngineError("DynamicTexture: failed to create texture");
    }

    return true;
}

DynamicTexture::LockHelper DynamicTexture::Lock(ContextPtr& context) {
    return LockHelper(m_device, context, m_texture);
}
