#pragma once

#include <cstdint>

#include "core/dg/dg.h"
#include "core/dg/object_base.h"


namespace Diligent {
    struct TextureDesc;
    class IReferenceCounters;
}

class DynamicTexture : public dg::ObjectBase<dg::IObject> {
public:
    DynamicTexture() = delete;
    DynamicTexture(dg::IReferenceCounters* refCounters, DevicePtr& device, const dg::TextureDesc& desc);
    ~DynamicTexture();

    TexturePtr Get() const noexcept;
    void SetSize(uint32_t width,  uint32_t height);

protected:
    DevicePtr m_device;
    TexturePtr m_texture;
};
