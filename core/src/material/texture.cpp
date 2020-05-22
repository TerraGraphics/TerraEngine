#include "core/material/texture.h"

#include "core/dg/device.h" // IWYU pragma: keep
#include "core/common/exception.h"


namespace {
    static void CheckIsCorrectTextureSize(uint32_t width,  uint32_t height) {
        if ((width == 0) || (height == 0)) {
            throw EngineError("DynamicTexture: size of texture must be non-zero");
        }
    }
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

void DynamicTexture::SetSize(uint32_t width,  uint32_t height) {
    auto desc = m_texture->GetDesc();

    if ((desc.Width == width) && (desc.Height == height)) {
        return;
    }

    desc.Width = width;
    desc.Height = height;
    CheckIsCorrectTextureSize(width, height);
    m_device->CreateTexture(desc, nullptr, &m_texture);
    if (!m_texture) {
        throw EngineError("DynamicTexture: failed to create texture");
    }
}
