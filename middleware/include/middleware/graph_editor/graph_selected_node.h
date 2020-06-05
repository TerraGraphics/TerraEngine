#pragma once

#include "dg/dg.h"
#include "core/common/ctor.h"


class GraphNode;
class INodePreview;
class SelectedNode : Fixed {
public:
    SelectedNode() = delete;
    SelectedNode(bool isOpenGL);
    ~SelectedNode();

    GraphNode* GetNode();
    void SetNode(GraphNode* node, INodePreview* previewNode);
    void ResetNode();
    void Draw();

private:
    bool m_isOpenGL = false;
    dg::RefCntWeakPtr<GraphNode> m_node;
    dg::RefCntAutoPtr<INodePreview> m_previewNode;
};
