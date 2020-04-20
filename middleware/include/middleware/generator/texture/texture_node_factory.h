#pragma once

#include "core/dg/dg.h"
#include "middleware/graph_editor/graph_node_factory.h"


class INodePreview;
class CoherentNoise;
class NoiseToTexture;
class PlaneProjection;
class TextureNodeFactory : public GraphNodeFactory {
public:
    TextureNodeFactory() = delete;
    TextureNodeFactory(DevicePtr& device, ContextPtr& context);
    ~TextureNodeFactory() override;

    CoherentNoise* CreateCoherentNoise();
    PlaneProjection* CreatePlaneProjection();
    NoiseToTexture* CreateNoiseToTexture();

    INodePreview* CompleteToPreview(CoherentNoise* node);
    INodePreview* CompleteToPreview(PlaneProjection* node);
    INodePreview* CompleteToPreview(NoiseToTexture* node);

    INodePreview* GetPreview(GraphNode* node) override;

private:
    DevicePtr m_device;
    ContextPtr m_context;
};
