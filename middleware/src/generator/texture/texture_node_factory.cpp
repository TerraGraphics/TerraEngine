#include "middleware/generator/texture/texture_node_factory.h"

#include <DiligentCore/Common/interface/DefaultRawMemoryAllocator.hpp>

#include "core/dg/render_device.h"
#include "core/dg/device_context.h"
#include "middleware/generator/texture/coherent_noise.h"
#include "middleware/generator/texture/noise_pojection.h"
#include "middleware/generator/texture/noise_rasterization.h"


TextureNodeFactory::TextureNodeFactory(DevicePtr& device, ContextPtr& context)
    : m_device(device)
    , m_context(context) {

}

TextureNodeFactory::~TextureNodeFactory() {
    m_device.Release();
    m_context.Release();
}

CoherentNoise* TextureNodeFactory::CreateCoherentNoise() {
    return NEW_OBJ(dg::DefaultRawMemoryAllocator::GetAllocator(), "CoherentNoise", CoherentNoise)();
}

PlaneProjection* TextureNodeFactory::CreatePlaneProjection() {
    return NEW_OBJ(dg::DefaultRawMemoryAllocator::GetAllocator(), "PlaneProjection", PlaneProjection)();
}

NoiseToTexture* TextureNodeFactory::CreateNoiseToTexture() {
    return NEW_OBJ(dg::DefaultRawMemoryAllocator::GetAllocator(), "NoiseToTexture", NoiseToTexture)(m_device, m_context);
}
