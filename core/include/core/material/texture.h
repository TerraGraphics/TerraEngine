#pragma once

#include <cstdint>

#include "core/dg/dg.h"
#include "core/common/ctor.h"
#include "core/dg/object_base.h"


namespace Diligent {
    struct TextureDesc;
    class IReferenceCounters;
}

class DynamicTexture : public dg::ObjectBase<dg::IObject> {
public:
    struct LockHelper : Fixed {
        LockHelper() = delete;
        LockHelper(DevicePtr& device, ContextPtr& context, TexturePtr& texture);
        ~LockHelper();

        uint8_t* data = nullptr;
        uint32_t stride = 0;
        const uint32_t width = 0;
        const uint32_t height = 0;

    private:
        uint8_t* m_updateData = nullptr;
        ContextPtr m_context;
        TexturePtr m_texture;
    };

public:
    DynamicTexture() = delete;
    DynamicTexture(dg::IReferenceCounters* refCounters, DevicePtr& device, const dg::TextureDesc& desc);
    ~DynamicTexture();

    TexturePtr Get() const noexcept;
    bool SetSize(uint32_t width,  uint32_t height);
    LockHelper Lock(ContextPtr& context);

protected:
    DevicePtr m_device;
    TexturePtr m_texture;
};
