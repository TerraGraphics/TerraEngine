#include "core/material/texture_manager.h"

#include <new>
#include <utility>
#include <type_traits>

#include "dg/device.h"
#include "core/material/texture.h"
#include "dg/default_raw_memory_allocator.h"


TextureManager::TextureManager(DevicePtr& device)
    : m_device(device) {

}

TextureManager::~TextureManager() {
    m_device.Release();
}

DynamicTexture* TextureManager::CreateDynamicTexture(dg::TEXTURE_FORMAT format, uint32_t width, uint32_t height, const char* name) {
    dg::TextureDesc desc;

    desc.Name = name;
    desc.Type = dg::RESOURCE_DIM_TEX_2D;
    desc.Width = width;
    desc.Height = height;
    desc.Format = format;
    desc.MipLevels = 0;
    desc.SampleCount = 1;
    desc.Usage = dg::USAGE_DYNAMIC;
    desc.BindFlags = dg::BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = dg::CPU_ACCESS_WRITE;
    desc.MiscFlags = dg::MISC_TEXTURE_FLAG_GENERATE_MIPS;
    desc.CommandQueueMask = 1;

    return NEW_OBJ(dg::DefaultRawMemoryAllocator::GetAllocator(), "DynamicTexture", DynamicTexture)(m_device, desc);
}
