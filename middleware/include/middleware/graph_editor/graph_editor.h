#pragma once

#include <memory>

#include "core/common/ctor.h"


namespace ax {
    namespace NodeEditor {
        struct EditorContext;
    }
}

class GraphNode;
class GraphStorage;
class GraphNodeFactory;
class GraphEditor : Fixed {
public:
    GraphEditor() = delete;
    GraphEditor(const std::string& name, std::unique_ptr<GraphNodeFactory>&& factory);
    ~GraphEditor();

    void Draw();

private:
    GraphNode* EditorMenu();

private:
    std::string m_name;
    ax::NodeEditor::EditorContext* m_context = nullptr;
    GraphStorage* m_storage = nullptr;
    std::unique_ptr<GraphNodeFactory> m_factory;
};
