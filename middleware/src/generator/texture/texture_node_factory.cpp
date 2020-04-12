#include "middleware/generator/texture/texture_node_factory.h"

#include <DiligentCore/Common/interface/DefaultRawMemoryAllocator.hpp>

#include "core/dg/render_device.h"
#include "core/dg/device_context.h"
#include "middleware/generator/texture/coherent_noise.h"
#include "middleware/generator/texture/noise_pojection.h"
#include "middleware/generator/texture/noise_rasterization.h"


TextureNodeFactory::TextureNodeFactory(DevicePtr& device, ContextPtr& context)
    : GraphNodeFactory ({
        { CoherentNoise::GetName(), [this]() { return CreateCoherentNoise(); }},
        { PlaneProjection::GetName(), [this]() { return CreatePlaneProjection(); }},
        { NoiseToTexture::GetName(), [this]() { return CreateNoiseToTexture(); }},
    })
    , m_device(device)
    , m_context(context) {

}

TextureNodeFactory::~TextureNodeFactory() {
    m_device.Release();
    m_context.Release();
}

CoherentNoise* TextureNodeFactory::CreateCoherentNoise() {
    return NEW_OBJ(dg::DefaultRawMemoryAllocator::GetAllocator(), CoherentNoise::GetName(), CoherentNoise)();
}

PlaneProjection* TextureNodeFactory::CreatePlaneProjection() {
    return NEW_OBJ(dg::DefaultRawMemoryAllocator::GetAllocator(), PlaneProjection::GetName(), PlaneProjection)();
}

NoiseToTexture* TextureNodeFactory::CreateNoiseToTexture() {
    return NEW_OBJ(dg::DefaultRawMemoryAllocator::GetAllocator(), NoiseToTexture::GetName(), NoiseToTexture)(m_device, m_context);
}
