#pragma once

#include "dg/dg.h"
#include "core/common/ctor.h"


class GraphNode;
class INodePreview;
class SelectedNode : Fixed {
public:
    SelectedNode();
    ~SelectedNode();

    GraphNode* GetNode();
    void SetNode(GraphNode* node, INodePreview* previewNode);
    void ResetNode();
    void Draw();

private:
    dg::RefCntWeakPtr<GraphNode> m_node;
    dg::RefCntAutoPtr<INodePreview> m_previewNode;
};
