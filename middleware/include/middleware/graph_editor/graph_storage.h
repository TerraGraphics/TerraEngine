#pragma once

#include <memory>
#include <vector>
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

struct LinkInfo {
    ax::NodeEditor::PinId srcPin;
    ax::NodeEditor::PinId dstPin;
};

class GraphNode;
class GraphStorage : Fixed {
public:
    GraphStorage();
    ~GraphStorage();

    void AddNode(GraphNode* node);
    bool AddLink(const ax::NodeEditor::PinId pinIdFirst, const ax::NodeEditor::PinId pinIdSecond, bool checkOnly);
    bool DelLink(const ax::NodeEditor::LinkId linkId, bool checkOnly);

    void Draw();

private:
    uintptr_t m_nextId = 1;
    std::vector<dg::RefCntAutoPtr<GraphNode>> m_nodes;
    std::unordered_map<ax::NodeEditor::LinkId, LinkInfo> m_links;
};
