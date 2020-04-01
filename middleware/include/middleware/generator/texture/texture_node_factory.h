#pragma once

#include "core/dg/dg.h"
#include "core/common/ctor.h"


class CoherentNoise;
class NoiseToTexture;
class PlaneProjection;
class TextureNodeFactory : Fixed {
public:
    TextureNodeFactory() = delete;
    TextureNodeFactory(DevicePtr& device, ContextPtr& context);
    ~TextureNodeFactory();

    CoherentNoise* CreateCoherentNoise();
    PlaneProjection* CreatePlaneProjection();
    NoiseToTexture* CreateNoiseToTexture();

private:
    DevicePtr m_device;
    ContextPtr m_context;
};
