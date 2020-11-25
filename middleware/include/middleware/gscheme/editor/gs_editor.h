#pragma once

#include <memory>
#include <string>
#include <cstdint>

#include "dg/dg.h"
#include "core/common/ctor.h"


namespace ax {
    namespace NodeEditor {
        struct Config;
        struct EditorContext;
    }
}

namespace gs {

class Draw;
class Graph;
class ClassStorage;
class Editor : Fixed {
public:
    Editor() = delete;
    Editor(const std::shared_ptr<ClassStorage>& classStorage, const std::string& name, TexturePtr& texBackground);
    ~Editor();

    void Create();
    void DrawGraph();
    void DrawNodeProperty();

private:
    void RemoveNode(uint16_t nodeId);
    void DrawNodeMenu(uint16_t nodeId);
    void DrawNewNodeMenu(float x, float y);

private:
    std::string m_name;
    ax::NodeEditor::Config* m_config = nullptr;
    ax::NodeEditor::EditorContext* m_context = nullptr;
    std::unique_ptr<Draw> m_draw;
    std::unique_ptr<Graph> m_graph;
    std::shared_ptr<ClassStorage> m_classStorage;
    uint16_t m_selectedNodeId = 0;

private:
    float m_menuPopupX = 0.f;
    float m_menuPopupY = 0.f;
    uint16_t m_menuNodeId = 0;
};

}
