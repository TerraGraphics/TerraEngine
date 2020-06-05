#pragma once

#include <memory>
#include <string>
#include <functional>

#include "dg/dg.h"
#include "core/common/ctor.h"


namespace ax {
    namespace NodeEditor {
        struct Config;
        struct EditorContext;
    }
}

class GraphNode;
class SelectedNode;
class GraphStorage;
class GraphNodeFactory;
class GraphEditor : Fixed {
public:
    GraphEditor() = delete;
    GraphEditor(const std::string& name, bool isGLDevice, TexturePtr& texBackground, std::unique_ptr<GraphNodeFactory>&& factory);
    ~GraphEditor();

    bool AddNode(GraphNode* node);
    void Draw();
    std::function<void ()> DrawProperty();

private:
    GraphNode* EditorMenu();

private:
    std::string m_name;
    ax::NodeEditor::Config* m_config = nullptr;
    ax::NodeEditor::EditorContext* m_context = nullptr;
    GraphStorage* m_storage = nullptr;
    std::unique_ptr<GraphNodeFactory> m_factory;
    std::shared_ptr<SelectedNode> m_selectedNode;
};
