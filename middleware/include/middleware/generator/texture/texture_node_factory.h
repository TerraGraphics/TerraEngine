#pragma once

#include "middleware/graph_editor/graph_node_factory.h"


class GraphNode;
class TextureNodeFactory : public GraphNodeFactory {
public:
    TextureNodeFactory();
    ~TextureNodeFactory() override;

    INodePreview* GetPreview(GraphNode* node) override;
};
