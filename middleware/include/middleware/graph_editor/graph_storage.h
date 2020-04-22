#pragma once

#include <memory>
#include <unordered_map>
#include <imgui_node_editor.h>

#include "core/dg/dg.h"
#include "core/common/ctor.h"


namespace std {
    template <> struct hash<ax::NodeEditor::LinkId> {
        size_t operator()(const ax::NodeEditor::LinkId& k) const {
            return size_t(k);
        }
    };
}

struct GraphPin;
struct LinkInfo {
    GraphPin* srcPin;
    GraphPin* dstPin;
};

class GraphNode;
class GraphStorage : Fixed {
public:
    GraphStorage() = delete;
    GraphStorage(TexturePtr& texBackground);
    ~GraphStorage();

    bool AddNode(GraphNode* node);
    bool DelNode(GraphNode* node, bool checkOnly);
    bool AddLink(GraphPin* pinFirst, GraphPin* pinSecond, bool checkOnly);
    bool DelLink(const ax::NodeEditor::LinkId linkId, bool checkOnly);

    GraphNode* GetSelectedNode() { return m_selectedNode; }

    void Draw(GraphNode* previewNode);

private:
    uintptr_t m_nextId = 1;
    float m_texBackgroundWidht = 1.f;
    float m_texBackgroundheight = 1.f;
    TextureViewPtr m_texBackground;
    GraphNode* m_selectedNode = nullptr;
    std::unordered_map<GraphNode*, dg::RefCntAutoPtr<GraphNode>> m_nodes;
    std::unordered_map<ax::NodeEditor::LinkId, LinkInfo> m_links;
};
