#pragma once

#include <cstdint>
#include <unordered_map>

#include "dg/dg.h"
#include "core/common/ctor.h"
#include "middleware/graph_editor/graph_node.h"


struct LinkInfo {
    GraphPin* srcPin;
    GraphPin* dstPin;
};

class GraphStorage : Fixed {
public:
    GraphStorage() = delete;
    GraphStorage(TexturePtr& texBackground);
    ~GraphStorage();

    bool AddNode(GraphNode* node);
    bool DelNode(GraphNode* node, bool checkOnly);
    bool AddLink(GraphPin* pinFirst, GraphPin* pinSecond, bool checkOnly);
    bool DelLink(uintptr_t linkId, bool checkOnly);

    GraphNode* GetSelectedNode() { return m_selectedNode; }

    void Draw(GraphNode* previewNode);

private:
    uintptr_t m_nextId = 1;
    float m_texBackgroundWidht = 1.f;
    float m_texBackgroundheight = 1.f;
    TextureViewPtr m_texBackground;
    GraphNode* m_selectedNode = nullptr;
    std::unordered_map<GraphNode*, dg::RefCntAutoPtr<GraphNode>> m_nodes;
    // LinkID -> LinkInfo
    std::unordered_map<uintptr_t, LinkInfo> m_links;
};
