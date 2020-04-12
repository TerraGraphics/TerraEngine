#pragma once

#include "core/dg/dg.h"
#include "middleware/graph_editor/graph_node_factory.h"


class CoherentNoise;
class NoiseToTexture;
class PlaneProjection;
class TextureNodeFactory : GraphNodeFactory {
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
