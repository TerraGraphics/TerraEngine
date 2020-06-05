#pragma once

#include <cstdint>

#include "dg/dg.h"
#include "core/common/ctor.h"


class DynamicTexture;
class TextureManager : Fixed {
public:
    TextureManager() = delete;
    TextureManager(DevicePtr& device);
    ~TextureManager();

    DynamicTexture* CreateDynamicTexture(dg::TEXTURE_FORMAT format, uint32_t width = 1, uint32_t height = 1, const char* name = nullptr);

private:
    DevicePtr m_device;
};
