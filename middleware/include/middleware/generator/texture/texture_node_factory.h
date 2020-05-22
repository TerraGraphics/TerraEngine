#pragma once

#include "middleware/graph_editor/graph_node_factory.h"


class GraphNode;
class INodePreview;
class CoherentNoise;
class NoiseToTexture;
class PlaneProjection;
class TextureNodeFactory : public GraphNodeFactory {
public:
    TextureNodeFactory();
    ~TextureNodeFactory() override;

    CoherentNoise* CreateCoherentNoise();
    PlaneProjection* CreatePlaneProjection();
    NoiseToTexture* CreateNoiseToTexture();

    INodePreview* CompleteToPreview(CoherentNoise* node);
    INodePreview* CompleteToPreview(PlaneProjection* node);
    INodePreview* CompleteToPreview(NoiseToTexture* node);

    INodePreview* GetPreview(GraphNode* node) override;
};
